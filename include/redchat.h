#ifndef REDCHAT_H
#define REDCHAT_H

#include <semaphore.h>

/* Valid return codes */
#define OK 0

/* Error return codes */
#define E_CANT_SPAWN_THREAD     1
#define E_CANT_ALLOC_BUFFER     2
#define E_INPUT_EMPTY           3
#define E_INPUT_TOO_LONG        4
#define E_CANT_CREATE_BARRIER   5
#define E_CANT_WAIT_ON_BARRIER  6
#define E_CANT_CREATE_SEMAPHORE 7

/* Number of units (threads) */
#define NUM_UNITS 3

/* Maximum number of queued messages */
#define MAX_QUEUED_MSGS 10

/* Barrier to keep all threads running concurrently and not exiting until
 * all threads are done */
extern pthread_barrier_t barr_all_done;

/* Message send queue */
extern char *send_queue;

/* Semaphore to keep client thread waiting until there's a queued message to
 * be sent */
extern sem_t queued_msgs;

/* Control variable to signalize threads whether the program is running or a
 * being requested to terminate */
extern int is_executing;

#endif
