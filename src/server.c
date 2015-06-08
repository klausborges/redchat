#include <stdio.h>
#include <pthread.h>
#include "../include/redchat.h"
#include "../include/utils.h"

void *server_unit() {
  int rc;

  debug(COLOR_YELLOW, "Server", "Waiting on barrier for all threads to load");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    fprintf(stderr, "Error waiting on barrier\n");
    return (void *) E_CANT_WAIT_ON_BARRIER;
  }

  debug(COLOR_YELLOW, "Server", "Starting");

  debug(COLOR_YELLOW, "Server", "Waiting on barrier");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    fprintf(stderr, "Error waiting on barrier\n");
    return (void *) E_CANT_WAIT_ON_BARRIER;
  }

  debug(COLOR_YELLOW, "Server", "Exiting");
  return OK;
}
