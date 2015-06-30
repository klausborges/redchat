#ifndef REDCHAT_H
#define REDCHAT_H

#include <semaphore.h>



/* Enable/disable debugging and colorized debug output. */
#define DEBUG           TRUE
#define DEBUG_COLORIZED TRUE

/* Valid return codes. */
#define OK 0

/* Error return codes. */
/* TODO: Reorder/regroup error codes */
#define E_CANT_SPAWN_THREAD          1
#define E_CANT_ALLOC_BUFFER          2
#define E_CANT_ALLOC_MESSAGE         3
#define E_INPUT_EMPTY                4
#define E_INPUT_TOO_LONG             5
#define E_CANT_CREATE_BARRIER        6
#define E_CANT_WAIT_ON_BARRIER       7
#define E_CANT_CREATE_SEMAPHORE      8
#define E_POST_WITH_EMPTY_QUEUE      9
#define E_COULDNT_JOIN_THREAD       10
#define E_CANT_ALLOC_CONTACT        11
#define E_CONTACT_LIST_FULL         12
#define E_CANT_CREATE_SOCKET        13
#define E_CANT_CONNECT_TO_SERVER    14
#define E_DEST_SERVER_OFFLINE       15
#define E_CANT_CREATE_SERVER_SOCKET 16
#define E_CANT_SET_SERVER_SOCKOPTS  17
#define E_CANT_BIND_SERVER_SOCKET   18
#define E_CANT_LISTEN_SERVER_SOCKET 19
#define E_INVALID_CONTACT_N         20
#define E_MESSAGE_STORAGE_FULL      21

/* String size constants. */
#define MAX_OPTION_SIZE   2
#define MAX_TIME_SIZE     8
#define MAX_NAME_SIZE     16
#define MAX_ADDRESS_SIZE  16
#define MAX_MESSAGE_SIZE  64
#define MAX_BUFFER_SIZE   64

/* Status codes for contacts. */
#define STATUS_UNKNOWN  0
#define STATUS_ONLINE   1
#define STATUS_OFFLINE  2

/* Number of units (threads). This is fixed in the implementation,
 * since there are three units (interactive, server and client). */
#define NUM_UNITS 3

/* Maximum number of queued messages for dispatching. */
#define MAX_QUEUED_MSGS 10

/* Maximum number of received messages to store. */
#define MAX_STORED_MSGS 10

/* Maximum number of contacts in address book. */
#define MAX_N_CONTACTS  10

/* Default port for the server listening socket. */
#define DEFAULT_SERVER_PORT 10000

/* Default number of maximum pending connections on server socket. */
#define MAX_PENDING_CONNECTIONS 5



/* Struct containing the address and the message to be sent. */
struct message {
  time_t time_sent;
  time_t time_received;
  short read;
  char *src_addr;
  char *dest_addr;
  char *text;
};

/* Struct containing the target address (online/offline). */
struct contact {
  char *address;
  char *alias;
  short status;
};

/* Barrier to keep all threads running concurrently and not exiting until
 * all threads are done executing/terminating. */
extern pthread_barrier_t all_done;

/* Queue for messages waiting to be dispatched. */
extern struct message *send_queue[];
extern int n_queued_msgs;

/* Storage for received messages. */
extern struct message *messages[];
extern int n_msgs;
extern int n_unread_msgs;

/* Array to hold all contacts information. */
extern struct contact *contacts[];
extern int n_contacts;

/* Semaphore to keep the client thread waiting until there's a queued message
 * to be dispatched. */
extern sem_t queued_msgs;

/* Control variable to signal threads whether the program is running or being
 * requested to terminate. */
extern int is_executing;

#endif
