#include "../includes/config.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../includes/job.h"

pid_t run_job(const char *command) {
  pid_t pid = fork();

  if (pid < 0) {
    perror("fork");
    _exit(-1);
  }

  if (pid == 0) {
    execl("/bin/sh", "sh", "-c", command, NULL);
    perror("execl");
    _exit(127);
  }

  return pid;
}
