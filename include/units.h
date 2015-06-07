#ifndef UNITS_H
#define UNITS_H

#define OK 0

#define E_THREAD_NOT_SPAWNED  1
#define E_BUF_MALLOC          2
#define E_NO_INPUT            3
#define E_INPUT_TOO_LONG      4

#define MAX_OPTION_SIZE   2
#define MAX_NAME_SIZE     16
#define MAX_MESSAGE_SIZE  128
#define MAX_BUFFER_SIZE   512

/* */
void *interactive_unit();

/* */
void *client_unit();

/* */
void *server_unit();

#endif
