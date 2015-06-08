#ifndef REDCHAT_H
#define REDCHAT_H

#include <semaphore.h>



/* Enable/disable debugging and colorized debug output. */
#define DEBUG           TRUE
#define DEBUG_COLORIZED TRUE

/* Valid return codes. */
#define OK 0

/* Error return codes. */
#define E_CANT_SPAWN_THREAD     1
#define E_CANT_ALLOC_BUFFER     2
#define E_INPUT_EMPTY           3
#define E_INPUT_TOO_LONG        4
#define E_CANT_CREATE_BARRIER   5
#define E_CANT_WAIT_ON_BARRIER  6
#define E_CANT_CREATE_SEMAPHORE 7
#define E_POST_WITH_EMPTY_QUEUE 8

/* String size constants. */
#define MAX_OPTION_SIZE   2
#define MAX_NAME_SIZE     16
#define MAX_MESSAGE_SIZE  128
#define MAX_BUFFER_SIZE   512

/* Number of units (threads). */
#define NUM_UNITS 3

/* Maximum number of queued messages. */
#define MAX_QUEUED_MSGS 10



/* Barrier to keep all threads running concurrently and not exiting until
 * all threads are done executing/terminating. */
extern pthread_barrier_t all_done;

/* Queue for messages waiting to be dispatched. */
extern char *send_queue[];
extern int n_queued_msgs;

/* Semaphore to keep the client thread waiting until there's a queued message
 * to be dispatched. */
extern sem_t queued_msgs;

/* Control variable to signal threads whether the program is running or being
 * requested to terminate. */
extern int is_executing;

#endif
