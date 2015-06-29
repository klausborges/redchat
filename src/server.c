#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "../include/redchat.h"
#include "../include/utils.h"



/* Internal functions for the server unit. */

/* Stores a received message. */
static int store_message(struct message *received_msg) {
  /* TODO: replace this faux message with the actual message */
  if (n_msgs == MAX_STORED_MSGS) {
    return E_MESSAGE_STORAGE_FULL;
  }

  /* Updates message with the time it was received and stores it */
  received_msg->time_received = time(NULL);
  messages[n_msgs] = received_msg;
  n_msgs++;
  n_unread_msgs++;

  return OK;
}



/* Server unit thread main function. */
void *server_unit() {
  int rc;
  int true_int = 1;
  int sock_server;
  int sin_size = sizeof(struct sockaddr_in);

  struct sockaddr_in server_addr, client_addr;

  /* Starts listening for connections on default port */
  if ((sock_server = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    debugerr(COLOR_YELLOW, "Server", "Couldn't create server socket");
    pthread_exit((void *) E_CANT_CREATE_SERVER_SOCKET);
  }

  /* Sets up the server socket */
  if (setsockopt(sock_server, SOL_SOCKET, SO_REUSEADDR, &true_int,
      sizeof(int)) == -1) {
    debugerr(COLOR_YELLOW, "Server", "Couldn't set socket options");
    pthread_exit((void *) E_CANT_SET_SERVER_SOCKOPTS);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(DEFAULT_SERVER_PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(server_addr.sin_zero), 8);

  if (bind(sock_server, (struct sockaddr *) &server_addr,
        sizeof(struct sockaddr)) == -1) {
    debugerr(COLOR_YELLOW, "Server", "Couldn't bind server socket");
    pthread_exit((void *) E_CANT_BIND_SERVER_SOCKET);
  }

  if (listen(sock_server, MAX_PENDING_CONNECTIONS) == -1) {
    debugerr(COLOR_YELLOW, "Server", "Couldn't listen to connections");
    pthread_exit((void *) E_CANT_LISTEN_SERVER_SOCKET);
  }

  /* TODO: Remove this */
  printf("Server listening on %s:%d\n", inet_ntoa(server_addr.sin_addr),
      DEFAULT_SERVER_PORT);

  /* TODO: remove faux message below */
  struct message *faux;
  faux = (struct message *) malloc(sizeof(struct message));
  faux->time_sent = time(NULL);
  faux->read = 0;
  faux->address = strndup("1.2.3.4", MAX_ADDRESS_SIZE);
  faux->text = strndup("Welcome to redchat!", MAX_MESSAGE_SIZE);
  store_message(faux);

  /* Waits on barrier for other units to load */
  debug(COLOR_YELLOW, "Server", "Waiting on barrier for units to load");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    debugerr(COLOR_YELLOW, "Server", "Error waiting on barrier");
    pthread_exit((void *) E_CANT_WAIT_ON_BARRIER);
  }

  debug(COLOR_YELLOW, "Server", "Starting");

  /* Execution loop, stays on it until an exit signal is signaled by
   * the interactive unit */
  while (is_executing) {
    debug(COLOR_YELLOW, "Server", "Waiting for connections");

    /* Blocking accept waits for connection attempts */
    /* TODO: assign to connection */
    accept(sock_server, (struct sockaddr *) &client_addr,
        (socklen_t *) &sin_size);
    printf("Client connected %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
  }

  /* Waits on barrier for all units to exit together */
  debug(COLOR_YELLOW, "Server", "Waiting on barrier for units to exit");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    debugerr(COLOR_YELLOW, "Server", "Error waiting on barrier");
    pthread_exit((void *) E_CANT_WAIT_ON_BARRIER);
  }

  debug(COLOR_YELLOW, "Server", "Exiting");
  pthread_exit(OK);
}
