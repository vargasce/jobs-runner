#include "../includes/job.h"
#include "../includes/utils.h"
#include <stdio.h>

static int job_id = 1;

int main() {

  FILE *file = fopen("jobs.conf", "r");
  if (!file) {
    perror("jobs.conf");
    return 1;
  }

  char command[1024];
  int exit_code;

  while (fgets(command, sizeof(command), file)) {
    trim_str_newline(command);

    if (command[0] == '\0')
      continue;

    printf("[%d] Running job: %s", job_id, command);

    exit_code = run_job(command);

    printf("[%d] Job finished with exit code: %d\n", job_id, exit_code);

    job_id++;
  }

  fclose(file);

  return 0;
}
