#include "../includes/job.h"
#include <stdio.h>

int main() {
  FILE *file = fopen("jobs.conf", "r");
  if (!file) {
    perror("jobs.conf");
    return 1;
  }

  char command[1024];

  if (!fgets(command, sizeof(command), file)) {
    fprintf(stderr, "jobs.conf is empty\n");
    fclose(file);
    return 1;
  }

  fclose(file);

  printf("Running job: %s", command);

  int exit_code = run_job(command);

  printf("Job finished with exit code: %d\n", exit_code);

  return exit_code;
}
