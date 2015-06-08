#ifndef SERVER_H
#define SERVER_H

/* Server thread functionality.
 * Handles listening for messages on a specified port and building a list
 * of messages to be handled by the interactive thread. */
void *server_unit();

#endif
