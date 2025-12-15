#pragma once

#include <string>
#include <vector>
#include <sys/types.h>
#include <ctime>

/* ---------------- Job State ---------------- */

enum JobState {
    RUNNING,
    STOPPED,
    DONE
};

/* ---------------- Job Structure ---------------- */

struct Job {
    int id;                 // [1], [2], ...
    pid_t pgid;             // process group id
    std::string cmd;        // full command
    JobState state;
    std::time_t startTime;
};

/* ---------------- Initialization ---------------- */

// Call once at shell startup
void init_shell();

/* ---------------- Job Launching ---------------- */

// Launch a job
// background = true  → cmd &
// background = false → foreground
//
// return:
//   0   → foreground job
//  >0   → background job id
//  -1   → error
int launch_job(const std::vector<std::string>& tokens,
               bool background,
               char** envp = nullptr);

/* ---------------- Job Table ---------------- */

void list_jobs();
void reap_done_jobs();

/* ---------------- SIGCHLD Processing ---------------- */

// MUST be called from main loop (never from signal handler)
void process_sigchld_events();

/* ---------------- Foreground / Background ---------------- */

int put_job_in_foreground(int jobId, bool cont);
int put_job_in_background(int jobId, bool cont);

/* ---------------- Utilities ---------------- */

int job_id_from_pgid(pid_t pgid);
