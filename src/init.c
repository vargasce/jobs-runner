#define _POSIX_C_SOURCE 200809L

#include "../includes/init.h"
#include "../includes/config.h"
#include "../includes/job.h"

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static int wait_job_with_timeout(RunningJob *rj) {
  int status;
  int exit_code = -1;

  while (1) {
    pid_t ret = waitpid(rj->pid, &status, WNOHANG);

    if (ret == rj->pid) {
      printf("terminó solo. \n");
      if (WIFEXITED(status))
        exit_code = WEXITSTATUS(status);
      else if (WIFSIGNALED(status))
        exit_code = 128 + WTERMSIG(status);

      return exit_code;
    }

    // no terminó aún
    if (rj->job->timeout > 0) {
      time_t elapsed = time(NULL) - rj->start;

      if (elapsed >= rj->job->timeout) {
        // timeout alcanzado
        kill(rj->pid, SIGTERM);
        sleep(1); // grace period
        kill(rj->pid, SIGKILL);

        waitpid(rj->pid, &status, 0);
        return 124; // código típico de timeout
      }
    }

    struct timespec ts = {.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000};
    nanosleep(&ts, NULL);
  }
}

int init_proccess() {

  JobList list = load_config("jobs.json");

  printf("Cantidad de jobs: %ld \n", list.count);

  for (size_t i = 0; i < list.count; i++) {
    printf("job: %s \n", list.jobs[i].name);

    RunningJob rj;
    rj.job = &list.jobs[i];
    rj.pid = run_job(list.jobs[i].command);
    rj.start = time(NULL);

    int exit_code = wait_job_with_timeout(&rj);

    printf("[%d] Job '%s' finished with exit code: %d\n", rj.pid, rj.job->name,
           exit_code);
  }

  clean_job(&list);

  return 0;
}
