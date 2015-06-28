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

static int dispatch_message(struct message *msg) {
  struct sockaddr_in saddr;
  struct hostent *host;
  int sock_client;

  host = gethostbyname(msg->address);

  if ((sock_client = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "Error creating socket\n");
    return E_CANT_CREATE_SOCKET;
  }

  saddr.sin_family = AF_INET;
  saddr.sin_port = DEFAULT_SERVER_PORT;
  saddr.sin_addr = *((struct in_addr *)host->h_addr);
  bzero(&(saddr.sin_zero), 8);
  /* TODO: is this better?
   * inet_pton(AF_INET, "127.0.0.1", &(saddr.sin_addr)); */

  /* Attempts a connection to destination server thread. */
  if (connect(sock_client, (struct sockaddr *) &saddr, sizeof(struct sockaddr))
      == -1) {
    /* TODO: Insert message here? */
    debugerr(COLOR_RED, "Client", "Destination server offline");
    return E_DEST_SERVER_OFFLINE;
  }

  printf("Message %s dispatched to %s\n", msg->text, msg->address);
}

void *client_unit() {
  int rc;
  struct message *deliver = NULL;

  debug(COLOR_RED, "Client", "Waiting on barrier for all threads to load");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    fprintf(stderr, "Error waiting on barrier\n");
    pthread_exit((void *) E_CANT_WAIT_ON_BARRIER);
  }

  debug(COLOR_RED, "Client", "Starting");

  while (is_executing) {
    debug(COLOR_RED, "Client", "Waiting for message to dispatch");
    sem_wait(&queued_msgs);
    if (n_queued_msgs > 0 && is_executing) {
      debug(COLOR_RED, "Client", "Processing message");

      deliver = send_queue[n_queued_msgs-1];
      printf("Sending message (%s) to (%s)\n", deliver->text,
          deliver->address);
      dispatch_message(deliver);

      free(deliver->address);
      free(deliver->text);
      free(deliver);
      n_queued_msgs--;

      debug(COLOR_RED, "Client", "Message sent");
    }
    else if (!is_executing) {
      debug(COLOR_RED, "Client", "Received termination signal");
      /* Free queued messages */
    }
    else {
      fprintf(stderr, "Post on queued_msgs semaphore without queued messages");
      pthread_exit((void *) E_POST_WITH_EMPTY_QUEUE);
    }
  }

  debug(COLOR_RED, "Client", "Waiting on barrier for all threads to finish");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    debugerr(COLOR_RED, "Client", "Can't wait on barrier");
    pthread_exit((void *) E_CANT_WAIT_ON_BARRIER);
  }

  debug(COLOR_RED, "Client", "Exiting");
  pthread_exit(OK);
}
