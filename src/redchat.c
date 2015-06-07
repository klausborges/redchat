#include <stdio.h>
#include <pthread.h>
#include "../include/units.h"
#include "../include/utils.h"

int main(int argc, char **argv) {
  pthread_t interactive_thread, client_thread, server_thread;

  /* Spawn client, server and interactive threads */
  printd("Main", "Spawning threads\n");

  if (pthread_create(&interactive_thread, NULL, &interactive_unit, NULL)) {
    fprintf(stderr, "Error creating interactive thread");
    return E_THREAD_NOT_SPAWNED;
  }

  if (pthread_create(&server_thread, NULL, &server_unit, NULL)) {
    fprintf(stderr, "Error creating interactive thread");
    return E_THREAD_NOT_SPAWNED;
  }

  if (pthread_create(&client_thread, NULL, &client_unit, NULL)) {
    fprintf(stderr, "Error creating interactive thread");
    return E_THREAD_NOT_SPAWNED;
  }

  printd("Main", "Waiting for threads to exit");

  pthread_join(interactive_thread, NULL);
  pthread_join(client_thread, NULL);
  pthread_join(server_thread, NULL);

  printd("Main", "Exiting\n");

  return 0;
}
