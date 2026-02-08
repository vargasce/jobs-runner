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

typedef struct {
  size_t total_jobs;
  size_t failed;
  size_t time_out;
  size_t success;
  size_t killed;
  time_t start;
} Metrics;

JobList load_config(const char *path);
void clean_job(JobList *jobs);

#endif // !CONFIG_H
