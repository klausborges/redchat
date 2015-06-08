#ifndef CLIENT_H
#define CLIENT_H

/* Client thread functionality.
 * Handles message dispatching from the queues built by the interactive
 * thread and managing the queue itself. */
void *client_unit();

#endif
