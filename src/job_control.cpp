#include "job_control.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include<cstring>
#include <iostream>
#include <vector>
#include <mutex>
#include <algorithm>

/* ============================================================
   Globals
   ============================================================ */

static std::vector<Job> jobs;
static std::mutex jobs_mtx;

static pid_t shell_pgid;
static int shell_fd;
static struct termios shell_tmodes;

static int sigchld_pipe[2];

/* ============================================================
   Forward Declarations
   ============================================================ */

static void init_sigchld_pipe();
static void setup_signals();
static void sigchld_handler(int);

/* ============================================================
   Helpers
   ============================================================ */

static int next_job_id() {
    int mx = 0;
    for (auto& j : jobs) mx = std::max(mx, j.id);
    return mx + 1;
}

static Job* find_job_by_pgid(pid_t pgid) {
    for (auto& j : jobs)
        if (j.pgid == pgid)
            return &j;
    return nullptr;
}

static Job* find_job_by_id(int id) {
    for (auto& j : jobs)
        if (j.id == id)
            return &j;
    return nullptr;
}

/* ============================================================
   SIGCHLD handler (ASYNC-SIGNAL-SAFE)
   ============================================================ */

struct SigchldMsg {
    pid_t pid;
    int status;
};

static void sigchld_handler(int) {
    int saved_errno = errno;
    SigchldMsg msg;

    while (true) {
        pid_t pid = waitpid(-1, &msg.status,
                            WNOHANG | WUNTRACED | WCONTINUED);
        if (pid <= 0) break;

        msg.pid = pid;
        write(sigchld_pipe[1], &msg, sizeof(msg));
    }

    errno = saved_errno;
}

/* ============================================================
   Initialization
   ============================================================ */

static void init_sigchld_pipe() {
    if (pipe(sigchld_pipe) < 0) {
        perror("pipe");
        _exit(1);
    }

    int flags = fcntl(sigchld_pipe[1], F_GETFL);
    fcntl(sigchld_pipe[1], F_SETFL, flags | O_NONBLOCK);

    flags = fcntl(sigchld_pipe[0], F_GETFL);
    fcntl(sigchld_pipe[0], F_SETFL, flags | O_NONBLOCK);
}

static void setup_signals() {
    struct sigaction sa{};
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGCHLD, &sa, nullptr);

    signal(SIGINT,  SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
}

void init_shell() {
    shell_fd = STDIN_FILENO;
    shell_pgid = getpid();

    while (tcgetpgrp(shell_fd) != shell_pgid) {
        kill(-shell_pgid, SIGTTIN);
    }

    setpgid(shell_pgid, shell_pgid);
    tcsetpgrp(shell_fd, shell_pgid);
    tcgetattr(shell_fd, &shell_tmodes);

    init_sigchld_pipe();
    setup_signals();
}

/* ============================================================
   SIGCHLD Processing (MAIN LOOP)
   ============================================================ */

void process_sigchld_events() {
    SigchldMsg msg;

    while (read(sigchld_pipe[0], &msg, sizeof(msg)) > 0) {
        std::lock_guard<std::mutex> lock(jobs_mtx);
        Job* j = find_job_by_pgid(getpgid(msg.pid));
        if (!j) continue;

        if (WIFSTOPPED(msg.status))
            j->state = STOPPED;
        else if (WIFCONTINUED(msg.status))
            j->state = RUNNING;
        else if (WIFEXITED(msg.status) || WIFSIGNALED(msg.status))
            j->state = DONE;
    }
}

/* ============================================================
   Job Launching
   ============================================================ */

int launch_job(const std::vector<std::string>& tokens,
               bool background,
               char** envp) {

    std::vector<char*> argv;
    for (auto& s : tokens)
        argv.push_back(strdup(s.c_str()));
    argv.push_back(nullptr);

    pid_t pid = fork();

    if (pid == 0) {
        setpgid(0, 0);
        if (!background)
            tcsetpgrp(shell_fd, getpid());

        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        execvp(argv[0], argv.data());
        perror("execvp");
        _exit(127);
    }

    setpgid(pid, pid);

    Job job;
    job.id = next_job_id();
    job.pgid = pid;
    job.state = RUNNING;
    job.startTime = time(nullptr);
    job.cmd = tokens[0];

    {
        std::lock_guard<std::mutex> lock(jobs_mtx);
        jobs.push_back(job);
    }

    for (char* p : argv) free(p);

    if (!background) {
        int status;
        waitpid(-pid, &status, WUNTRACED);
        tcsetpgrp(shell_fd, shell_pgid);
        return 0;
    }

    std::cout << "[" << job.id << "] " << job.pgid << "\n";
    return job.id;
}

/* ============================================================
   Job Control Commands
   ============================================================ */

void list_jobs() {
    std::lock_guard<std::mutex> lock(jobs_mtx);
    for (auto& j : jobs) {
        std::cout << "[" << j.id << "] "
                  << (j.state == RUNNING ? "Running" :
                      j.state == STOPPED ? "Stopped" : "Done")
                  << "  " << j.cmd << "\n";
    }
}

int put_job_in_foreground(int jobId, bool cont) {
    Job* j = find_job_by_id(jobId);
    if (!j) return -1;

    if (cont) kill(-j->pgid, SIGCONT);

    tcsetpgrp(shell_fd, j->pgid);
    int status;
    waitpid(-j->pgid, &status, WUNTRACED);
    tcsetpgrp(shell_fd, shell_pgid);
    return 0;
}

int put_job_in_background(int jobId, bool cont) {
    Job* j = find_job_by_id(jobId);
    if (!j) return -1;

    if (cont) kill(-j->pgid, SIGCONT);
    j->state = RUNNING;
    return 0;
}

void reap_done_jobs() {
    std::lock_guard<std::mutex> lock(jobs_mtx);
    jobs.erase(
        std::remove_if(jobs.begin(), jobs.end(),
            [](const Job& j) { return j.state == DONE; }),
        jobs.end());
}
