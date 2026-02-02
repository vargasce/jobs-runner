#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <sys/types.h>

#include "../includes/job.h"

typedef struct {
  Job *jobs;
  size_t count;
} JobList;

typedef struct {
  Job *job;
  pid_t pid;
  time_t start;
} RunningJob;

JobList load_config(const char *path);
void clean_job(JobList *jobs);

#endif // !CONFIG_H
