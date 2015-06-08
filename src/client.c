#include <stdio.h>
#include <pthread.h>
#include "../include/redchat.h"
#include "../include/client.h"
#include "../include/utils.h"

void *client_unit() {
  int rc;

  debug(COLOR_RED, "Client", "Waiting on barrier for all threads to load");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    fprintf(stderr, "Error waiting on barrier\n");
    return (void *) E_CANT_WAIT_ON_BARRIER;
  }

  debug(COLOR_RED, "Client", "Starting");

  while (is_executing) {
    sem_wait(&queued_msgs);
    if (n_queued_msgs > 0 && is_executing) {
      debug(COLOR_RED, "Client", "Processing message");

      printf("Sending message (%s)\n", send_queue[n_queued_msgs-1]);
      /* TODO: free queued msg alloced string? */
      send_queue[n_queued_msgs-1] = NULL;
      n_queued_msgs--;

      debug(COLOR_RED, "Client", "Message sent");
    }
    else if (!is_executing) {
      debug(COLOR_RED, "Client", "Received termination signal");
      /* Free queued messages */
    }
    else {
      fprintf(stderr, "Post on queued_msgs semaphore without queued messages");
      return (void *) E_POST_WITH_EMPTY_QUEUE;
    }
  }

  debug(COLOR_RED, "Client", "Waiting on barrier for all threads to finish");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    debugerr(COLOR_RED, "Client", "Can't wait on barrier");
    return (void *) E_CANT_WAIT_ON_BARRIER;
  }

  debug(COLOR_RED, "Client", "Exiting");
  return OK;
}
