#ifndef JOB_H
#define JOB_H

#include <sys/types.h>

typedef struct {
  char *name;
  char *command;
  char *path;
  int timeout;
} Job;

pid_t run_job(const char *command);

#endif
