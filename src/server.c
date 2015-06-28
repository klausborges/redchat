#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "../include/redchat.h"
#include "../include/utils.h"



/* Server unit thread main function. */
void *server_unit() {
  int rc;
  int true_int = 1;
  int sock_server, connection, sin_size;
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
    connection = accept(sock_server, (struct sockaddr *) &client_addr,
        (socklen_t *) &sin_size);
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
