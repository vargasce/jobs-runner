#include <stddef.h>
#define _POSIX_C_SOURCE 200809L

#include "../includes/config.h"
#include "../includes/init.h"
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

int init_proccess() {
  init_handle_sigint();

  JobList list = load_config("jobs.json");

  printf("Cantidad de jobs: %ld \n", list.count);

  RunningJob running[MAX_PARALLEL];
  size_t runing_count = 0;
  size_t next_job = 0;

  while ((next_job < list.count || runing_count > 0) && !stop_requested) {

    while (next_job < list.count && runing_count < MAX_PARALLEL) {
      RunningJob *rj = &running[runing_count];

      rj->job = &list.jobs[next_job];
      rj->pid = run_job(rj->job->command);
      rj->start = time(NULL);

      printf("[START] %s pid=%d\n", rj->job->name, rj->pid);

      runing_count++;
      next_job++;
    }

    for (size_t i = 0; i < runing_count; i++) {
      int status;
      pid_t ret = waitpid(running[i].pid, &status, WNOHANG);

      // Sigue ejecutando
      if (ret == 0) {
        // chequeo timeout
        if (running[i].job->timeout > 0) {
          time_t elapsed = time(NULL) - running[i].start;
          if (elapsed >= running[i].job->timeout) {
            printf("[TIMEOUT] %s pid=%d\n", running[i].job->name,
                   running[i].pid);

            kill_process(&running[i]);

            running[i] = running[runing_count - 1];
            runing_count--;
            i--;
          }
        }
        continue;
      }

      // Terminó
      if (ret == running[i].pid) {
        int exit_code;

        if (WIFEXITED(status))
          exit_code = WEXITSTATUS(status);
        else
          exit_code = 128 + WTERMSIG(status);

        printf("[END] %s pid=%d exit=%d\n", running[i].job->name,
               running[i].pid, exit_code);

        running[i] = running[runing_count - 1];
        runing_count--;
        i--;
      }
    }

    struct timespec ts = {.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000};
    nanosleep(&ts, NULL);

    if (stop_requested) { // press Ctrl + c
      for (size_t i = 0; i < runing_count; i++) {
        kill_process(&running[i]);
      }
    }
  }

  clean_job(&list);

  return stop_requested ? 130 : 0;
}
