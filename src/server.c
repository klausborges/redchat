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
static int store_message(struct message received_msg, char *src_address) {
  struct stored_message *stored_msg = NULL;

  if (n_msgs == MAX_STORED_MSGS) {
    return E_MESSAGE_STORAGE_FULL;
  }

  /* Allocates memory for the received message and copies data */
  stored_msg = (struct stored_message *) malloc(sizeof(struct stored_message));
  if (!stored_msg) {
    return E_CANT_ALLOC_MESSAGE;
  }

  /* Translates message struct to stored_message struct, adding the extra
   * information necessary */
  /* TODO: Fix broken non null-terminated strings */
  stored_msg->time_sent = received_msg.time_sent;
  stored_msg->time_received = time(NULL);
  stored_msg->read = FALSE;
  stored_msg->src_address = strndup(src_address, MAX_ADDRESS_SIZE);
  /* Missing non terminated string here! Message is received but can't
   * be passed!
   * stored_msg->text = strndup(received_msg.text, MAX_MESSAGE_SIZE);
   */

  /* Stores the message */
  messages[n_msgs] = stored_msg;
  n_msgs++;
  n_unread_msgs++;

  return OK;
}



/* Server unit thread main function. */
void *server_unit() {
  int rc;
  int true_int = 1;
  int sock_server, connection;
  int sin_size = sizeof(struct sockaddr_in);
  struct sockaddr_in server_addr, client_addr;
  struct message received_msg;
  char *client_addr_string = NULL;

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

  /* Binds the address */
  if (bind(sock_server, (struct sockaddr *) &server_addr,
        sizeof(struct sockaddr)) == -1) {
    debugerr(COLOR_YELLOW, "Server", "Couldn't bind server socket");
    pthread_exit((void *) E_CANT_BIND_SERVER_SOCKET);
  }

  /* Listens for connections on default port */
  if (listen(sock_server, MAX_PENDING_CONNECTIONS) == -1) {
    debugerr(COLOR_YELLOW, "Server", "Couldn't listen to connections");
    pthread_exit((void *) E_CANT_LISTEN_SERVER_SOCKET);
  }

  /* TODO: Maybe remove welcome message? */
  struct message welcome;
  welcome.time_sent = time(NULL);
  welcome.type = MSG_TYPE_TEXT;
  welcome.dest_address = strndup("0.0.0.0", MAX_ADDRESS_SIZE);
  welcome.text = strndup("Welcome to redchat!", MAX_MESSAGE_SIZE);
  store_message(welcome, "0.0.0.0");

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

    /* Waits for a connection and reads the message sent */
    connection = accept(sock_server, (struct sockaddr *) &client_addr,
        (socklen_t *) &sin_size);
    debug(COLOR_YELLOW, "Server", "Client connected");

    client_addr_string = inet_ntoa(client_addr.sin_addr);
    recv(connection, (void *) &received_msg, sizeof(received_msg), 0);
    debug(COLOR_YELLOW, "Server", "Message received");

    /* Puts received message on storage if it is a text message */
    if (received_msg.type == MSG_TYPE_TEXT) {
      store_message(received_msg, client_addr_string);
    }
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
