#include "../includes/init.h"
#include "../includes/config.h"
#include "../includes/job.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int init_proccess() {

  JobList list = load_config("jobs.json");

  printf("Cantidad de elementos: %ld \n", list.count);

  for (size_t i = 0; i < list.count; i++) {
    printf("job: %s \n", list.jobs[i].name);

    RunningJob rj;
    rj.job = &list.jobs[i];
    rj.pid = run_job(list.jobs[i].command);
    rj.start = time(NULL);

    int status;
    waitpid(rj.pid, &status, 0);

    int exit_code = -1;
    if (WIFEXITED(status))
      exit_code = WEXITSTATUS(status);

    printf("[%d] Job '%s' finished with exit code: %d\n", rj.pid, rj.job->name,
           exit_code);
  }

  return 0;
}
