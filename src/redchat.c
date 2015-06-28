#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "../include/redchat.h"
#include "../include/server.h"
#include "../include/client.h"
#include "../include/interact.h"
#include "../include/utils.h"

pthread_barrier_t all_done;
sem_t queued_msgs;

struct message *send_queue[MAX_QUEUED_MSGS];
struct message *messages[MAX_STORED_MSGS];
struct contact *contacts[MAX_N_CONTACTS];

int is_executing;
int n_queued_msgs;
int n_contacts;
int n_msgs;

int main(int argc, char **argv) {
  int rc;
  void *rs = NULL;
  pthread_t interactive_thread, client_thread, server_thread;

  /* Initialize control variables */
  n_queued_msgs = 0;
  n_msgs = 0;
  n_contacts = 0;
  is_executing = 1;

  /* Initialize barrier */
  debug(COLOR_BLUE, "Main", "Initializing barrier");
  if (pthread_barrier_init(&all_done, NULL, NUM_UNITS)) {
    return E_CANT_CREATE_BARRIER;
  }
  debug(COLOR_BLUE, "Main", "Barrier initialized");

  /* Initialize send queue semaphore */
  if (sem_init(&queued_msgs, 0, 0)) {
    fprintf(stderr, "Error initializing semaphore\n");
    return E_CANT_CREATE_SEMAPHORE;
  }

  /* Spawn client, server and interactive threads */
  debug(COLOR_BLUE, "Main", "Spawning threads");

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

  /* Joins all threads and checks for errors */
  /* TODO: proper return code handling */
  debug(COLOR_BLUE, "Main", "Waiting for threads to exit");
  rc = pthread_join(interactive_thread, rs);
  if (rc) {
    debugerr(COLOR_BLUE, "Main", "Error from pthread_join interactive thread");
    return E_COULDNT_JOIN_THREAD;
  }
  else {
    debug(COLOR_BLUE, "Main", "Interactive thread joined");
  }

  rc = pthread_join(client_thread, rs);
  if (rc) {
    debugerr(COLOR_BLUE, "Main", "Error from pthread_join client thread");
    return E_COULDNT_JOIN_THREAD;
  }
  else {
    debug(COLOR_BLUE, "Main", "Client thread joined");
  }

  rc = pthread_join(server_thread, rs);
  if (rc) {
    debugerr(COLOR_BLUE, "Main", "Error from pthread_join server thread");
    return E_COULDNT_JOIN_THREAD;
  }
  else {
    debug(COLOR_BLUE, "Main", "Server thread joined");
  }

  /* Frees allocated resources and exits */
  debug(COLOR_BLUE, "Main", "Exiting\n");
  sem_destroy(&queued_msgs);
  pthread_barrier_destroy(&all_done);

  return EXIT_SUCCESS;
}
