/**
 * An example SystemD daemon.
 */

#include <stdbool.h>    // bool, true, false macros
#include <stdlib.h>     // exit, EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>      // printf, fprintf

#include <unistd.h>     // sleep
#include <signal.h>     // sigaction, signal

/**
 * Whether the daemon should continue running.
 */
bool running = false;

/**
 * Process a received signal.
 */
void handle_signal(int sig) {
  switch (sig) {
    // sent by process manager to indicate we should reload
    case SIGHUP:
      // XXX: reload daemon configuration files and reopen file descriptors
      printf("ignoring SIGHUP because there is nothing to do\n");
      break;

    // sent by process manager to indicate we should stop gracefully
    case SIGTERM:
      printf("received SIGTERM and clearing the run flag\n");
      running = false;
      break;

    // sent by user hitting Ctrl + C in the terminal
    case SIGINT:
      printf("received SIGINT and clearing the run flag\n");
      running = false;
      break;
  }
}

/**
 * Application entry point.
 *
 * @param argc number of arguments
 * @param argv argument data
 * @returns exit code
 */
int main(int argc, char* argv[]) {
  // set the running flag
  running = true;

  // register the signal handler for relevant signals
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = handle_signal;

  if (sigaction(SIGHUP, &sa, NULL) < 0) {
    fprintf(stderr, "failed to install SIGHUP signal handler\n");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGTERM, &sa, NULL) < 0) {
    fprintf(stderr, "failed to install SIGTERM signal handler\n");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGINT, &sa, NULL) < 0) {
    fprintf(stderr, "failed to install SIGINT signal handler\n");
    exit(EXIT_FAILURE);
  }

  // report our status
  printf("starting\n");

  // do useful work
  long a = 0;
  long b = 1;

  while (running) {
    // sleep for a while
    sleep(1);

    // compute the next term
    long sum = a + b;
    a = b;
    b = sum;

    // report the term
    printf("computed term: %ld\n", a);
  }

  // report our status
  printf("stopping\n");

  // exit cleanly
  exit(EXIT_SUCCESS);
}
