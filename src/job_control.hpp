#pragma once 
#include<string>
#include<vector>
#include<sys/types.h>
#include<ctime>

enum JobState{RUNNING,STOPPED,DONE};

struct Job{
    int id;               //JOB id
    pid_t pgid;           //process group id
    std::string cmd;      //command
    JobState state;       //RUNNING
    std::time_t startTime;//start time 
};

void init_shell();  //call at startup to initailze signal handling, signit

// Launch a job given tokenized command (tokens vector) and whether background
// returns job id (>=1) for background jobs, or 0 for foreground jobs (already waited)

int launch_jobs(const std::vector<std::string>& tokens,bool background,char ** env = nullptr);


//Job table helper

void list_job();
void mark_job_status(pid_t pid,int status);
void reap_done_jobs();

// Foreground / Background control

int put_job_in_foreground(int JobId,bool cont);
int put_job_in_background(int JobId,bool cont);

// Utility to find job by id
int job_id_from_pgid(pid_t pgid);



