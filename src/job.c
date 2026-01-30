#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../includes/job.h"

int run_job(const char *command) {
  pid_t pid = fork();

  if (pid < 0) {
    perror("fork");
    return -1;
  }

  if (pid == 0) {
    execl("/bin/sh", "sh", "-c", command, NULL);
    perror("execl");
    exit(1);
  }

  int status;
  waitpid(pid, &status, 0); // Espera que termine pid

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }

  return 1;
}
