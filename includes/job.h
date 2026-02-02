#ifndef JOB_H
#define JOB_H

typedef struct {
  char *name;
  char *command;
  char *path;
  int timeout;
} Job;

int run_job(const char *command);

#endif
