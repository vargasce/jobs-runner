#define _POSIX_C_SOURCE 200809L

#include "../includes/init.h"
#include "../includes/config.h"
#include "../includes/job.h"

#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

volatile sig_atomic_t stop_requested = 0;

void handle_sigint(int sig) {
  (void)sig;
  stop_requested = 1;
}

static void init_handle_sigint() {
  struct sigaction sa = {0};
  sa.sa_handler = handle_sigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  sigaction(SIGINT, &sa, NULL);
}

static void kill_process(RunningJob *rj) {
  kill(rj->pid, SIGTERM);
  sleep(1);
  kill(rj->pid, SIGKILL);
  waitpid(rj->pid, NULL, 0);
}

static int wait_job_with_timeout(RunningJob *rj) {
  int status;
  int exit_code = -1;

  if (stop_requested) {
    kill_process(rj);
    return 130;
  }

  while (1) {
    pid_t ret = waitpid(rj->pid, &status, WNOHANG);

    if (ret == rj->pid) {
      if (WIFEXITED(status))
        exit_code = WEXITSTATUS(status);
      else if (WIFSIGNALED(status))
        exit_code = 128 + WTERMSIG(status);

      return exit_code;
    }

    // no terminó aún
    if (rj->job->timeout > 0) {
      time_t elapsed = time(NULL) - rj->start;

      if (elapsed >= rj->job->timeout) { // timeout alcanzado
        if (waitpid(rj->pid, NULL, WNOHANG) == 0) {
          kill_process(rj);
        }
        return 124; // código timeout
      }
    }

    struct timespec ts = {.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000};
    nanosleep(&ts, NULL);
  }
}

int init_proccess() {
  init_handle_sigint();

  JobList list = load_config("jobs.json");

  printf("Cantidad de jobs: %ld \n", list.count);

  for (size_t i = 0; i < list.count; i++) {

    if (stop_requested) {
      printf("Ctrl+C recibido. Terminando jobs...\n");
      break;
    }

    printf("job: %s \n", list.jobs[i].name);

    RunningJob rj;
    rj.job = &list.jobs[i];
    rj.pid = run_job(list.jobs[i].command);
    rj.start = time(NULL);

    int exit_code = wait_job_with_timeout(&rj);

    time_t end = time(NULL);

    if (stop_requested) {
      printf("Job interrumpido por Ctrl+C\n");
    } else {
      printf("[%d] Job '%s' finished with exit code: %d in %lds\n", rj.pid,
             rj.job->name, exit_code, end - rj.start);
    }
  }

  clean_job(&list);

  return 0;
}
