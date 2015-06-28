#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/redchat.h"
#include "../include/client.h"
#include "../include/utils.h"



/* Internal functions for the client unit. */

/* Dispatches queued messages to their recipients. */
static int dispatch_message(struct message *msg) {
  struct sockaddr_in server_addr;
  struct hostent *host;
  int sock_client;

  /* Sets up the socket */
  host = gethostbyname(msg->address);

  if ((sock_client = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    return E_CANT_CREATE_SOCKET;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = DEFAULT_SERVER_PORT;
  server_addr.sin_addr = *((struct in_addr *)host->h_addr);
  bzero(&(server_addr.sin_zero), 8);
  /* TODO: is this better?
   * inet_pton(AF_INET, "ip_address", &(server_addr.sin_addr)); */

  /* Attempts a connection to destination server unit */
  if (connect(sock_client, (struct sockaddr *) &server_addr,
        sizeof(struct sockaddr)) == -1) {
    return E_DEST_SERVER_OFFLINE;
  }

  return OK;
}



/* Client unit thread main function. */
void *client_unit() {
  int rc;
  struct message *deliver = NULL;

  /* Waits on barrier for other units to load */
  debug(COLOR_RED, "Client", "Waiting on barrier for units to load");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    debugerr(COLOR_RED, "Client", "Error waiting on barrier");
    pthread_exit((void *) E_CANT_WAIT_ON_BARRIER);
  }

  debug(COLOR_RED, "Client", "Starting");

  /* Execution loop, stays on it until an exit signal is signaled by
   * the interactive unit */
  while (is_executing) {
    /* Waits on semaphore for queued messages */
    debug(COLOR_RED, "Client", "Waiting for message to dispatch");
    sem_wait(&queued_msgs);

    if (n_queued_msgs > 0 && is_executing) {
      /* Attempts to deliver the message */
      debug(COLOR_RED, "Client", "Attempting to dispatch message");

      deliver = send_queue[n_queued_msgs-1];
      rc = dispatch_message(deliver);
      if (rc == OK) {
        debug(COLOR_RED, "Client", "Message dispatched");
      }
      else if (rc == E_CANT_CREATE_SOCKET) {
        debugerr(COLOR_RED, "Client", "Couldn't create socket");
        pthread_exit((void *) E_CANT_CREATE_SOCKET);
      }
      else if (rc == E_DEST_SERVER_OFFLINE) {
        debugerr(COLOR_RED, "Client", "Destination server not found");
        /* TODO: Mark contact as offline, remove contact, retry sending msg? */
      }

      /* Frees dispatched message resources */
      free(deliver->address);
      free(deliver->text);
      free(deliver);
      n_queued_msgs--;
    }
    else if (!is_executing) {
      debug(COLOR_RED, "Client", "Received termination signal");
      /* TODO: Free queued messages and exit? */
    }
    else {
      debugerr(COLOR_RED, "Client", "Post on semaphore with empty queue");
      pthread_exit((void *) E_POST_WITH_EMPTY_QUEUE);
    }
  }

  /* Waits on barrier for all units to exit together */
  debug(COLOR_RED, "Client", "Waiting on barrier for units to exit");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    debugerr(COLOR_RED, "Client", "Error waiting on barrier");
    pthread_exit((void *) E_CANT_WAIT_ON_BARRIER);
  }

  debug(COLOR_RED, "Client", "Exiting");
  pthread_exit(OK);
}
