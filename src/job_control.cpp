#include "job_control.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>
#include <cstring>
#include <algorithm>
#include <mutex>

static std::vector<Job> jobs;
static std::mutex jobs_mtx;
static pid_t shell_pgid;
static struct termios shell_termios;
static int shell_terminal_fd = -1;
static int sigchild_pipe[2] = {-1,-1};

//Internal helper
static void add_job(const Job& job){
    std::lock_guard<std::mutex>lock(jobs_mtx);
    jobs.push_back(job);
}

static Job* find_job_by_id(int id){
    for(auto& job:jobs)if(job.id == id)return &job;
    return nullptr;
}

static Job* find_job_by_pid(int pgid){
    for(auto& job:jobs)if(job.pgid == pgid)return &job;
    return nullptr;
}

static int next_job_id(){
    int mx = 0;
    for(auto& job:jobs)if(job.id>mx)mx = job.id;
    return mx+1;
}

static void print_job(const Job& j){
    const char* state = (j.state==RUNNING?"RUNNING" : (j.state == STOPPED?"STOPPED":"DONE"));
    std::cout<<"["<<j.id<<"] "<<state<<"\t"<<j.cmd<<"(pgid="<<j.pgid<<")\n";
}

//Signal handling 
struct SigchldMsg{
    pid_t pid;
    int status;
};

/*This function is a SIGCHLD handler.
SIGCHLD is sent to a parent process whenever:

A child exits

A child is stopped (Ctrl+Z)

A child continues (after SIGCONT)

A child is terminated by a signal
*/
static void sigchld_handler(int sig){
    int saved_errno = errno;
    pid_t pid;
    int status;
    SigchldMsg msg;
    while((pid == waitpid(-1,&status,WNOHANG | WUNTRACED | WCONTINUED))>0){
        msg.pid = pid;
        msg.status = status;
        ssize_t r = write(sigchild_pipe[1],&msg,sizeof(msg));
        (void)r;
    }

    errno = saved_errno;
}



