#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "../include/redchat.h"
#include "../include/units.h"
#include "../include/utils.h"

pthread_barrier_t barr_all_done;

sem_t queued_msgs;

int is_executing;

int main(int argc, char **argv) {
  pthread_t interactive_thread, client_thread, server_thread;

  /* Set control variables */
  is_executing = 1;

  /* Initialize barrier */
  printd("Main", "Initializing barrier");
  if (pthread_barrier_init(&barr_all_done, NULL, NUM_UNITS)) {
    return E_CANT_CREATE_BARRIER;
  }
  printd("Main", "Barrier initialized");

  /* Initialize send message queue */
  char *queue[MAX_QUEUED_MSGS];

  /* Initialize send queue semaphore */
  if (sem_init(&queued_msgs, 0, 0)) {
    fprintf(stderr, "Error initializing semaphore\n");
    return E_CANT_CREATE_SEMAPHORE;
  }

  /* Spawn client, server and interactive threads */
  printd("Main", "Spawning threads\n");

  if (pthread_create(&interactive_thread, NULL, &interactive_unit, NULL)) {
    fprintf(stderr, "Error creating interactive thread");
    return E_CANT_SPAWN_THREAD;
  }

  if (pthread_create(&server_thread, NULL, &server_unit, NULL)) {
    fprintf(stderr, "Error creating interactive thread");
    return E_CANT_SPAWN_THREAD;
  }

  if (pthread_create(&client_thread, NULL, &client_unit, NULL)) {
    fprintf(stderr, "Error creating interactive thread");
    return E_CANT_SPAWN_THREAD;
  }

  printd("Main", "Waiting for threads to exit");

  pthread_join(interactive_thread, NULL);
  pthread_join(client_thread, NULL);
  pthread_join(server_thread, NULL);

  printd("Main", "Exiting\n");
  sem_destroy(&queued_msgs);

  return 0;
}
