#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../include/redchat.h"
#include "../include/utils.h"



/* Internal functions for the interactive unit. */

/* Prints the menu. */
static void print_menu() {
  printf("1. Add contact IP\n");
  printf("2. List contacts\n");
  printf("3. Remove contact\n");
  printf("4. Send message\n");
  printf("5. Read messages (%d)\n", n_msgs);
  printf("6. Exit\n");
  printf("Option> ");
}



/* Prints the contact list with padding and alignment. */
static void print_contacts() {
  int i;

  if (n_contacts == 0) {
    printf("\n  Contact list is empty!\n\n");
  }
  else {
    printf("\n  %*s %*s %*s\n", 2, "n", MAX_NAME_SIZE, "Alias",
           MAX_ADDRESS_SIZE, "Address");
    for (i = 0; i < n_contacts; i++) {
      printf("  %*d %*s %*s\n", 2, i, MAX_NAME_SIZE, contacts[i]->alias,
          MAX_ADDRESS_SIZE, contacts[i]->address);
    }
    printf("\n");
  }
}



/* Adds a contact to the contact list, reading an address and an alias. */
static int add_contact() {
  if (n_contacts == MAX_N_CONTACTS) {
    printf("\n  Contact list is full!\n\n");
    return E_CONTACT_LIST_FULL;
  }
  else {
    char *buffer = NULL;
    struct contact *new_contact = NULL;

    /* Allocates memory for buffer and contact struct */
    buffer = (char *) malloc(MAX_ADDRESS_SIZE * sizeof(char));
    if (!buffer) {
      return E_CANT_ALLOC_BUFFER;
    }

    new_contact = (struct contact *) malloc(sizeof(struct contact));
    if (!new_contact) {
      return E_CANT_ALLOC_CONTACT;
    }

    /* Reads input and strips newlines */
    printf("\n     IP: ");
    read_line(buffer, MAX_ADDRESS_SIZE);
    strip(buffer, MAX_ADDRESS_SIZE);
    new_contact->address = strndup(buffer, MAX_ADDRESS_SIZE-1);

    printf("  Alias: ");
    read_line(buffer, MAX_NAME_SIZE);
    strip(buffer, MAX_NAME_SIZE);
    new_contact->alias = strndup(buffer, MAX_NAME_SIZE-1);

    /* Assigns contact online/offline status to unknown */
    new_contact->status = STATUS_UNKNOWN;

    /* Adds new contact to the end of the list */
    contacts[n_contacts] = new_contact;
    n_contacts++;

    /* Frees allocated resources */
    free(buffer);

    printf("\n");
    return OK;
  }
}



/* Reads a message and its destination and queues it to be handled by the
 * client unit. */
static int queue_message() {
  char *buffer = NULL;
  struct message *msg = NULL;

  /* Allocates memory for buffer and message struct */
  buffer = (char *) malloc(MAX_BUFFER_SIZE * sizeof(char));
  if (!buffer) {
    return E_CANT_ALLOC_BUFFER;
  }

  msg = (struct message *) malloc(sizeof(struct message));
  if (!msg) {
    return E_CANT_ALLOC_MESSAGE;
  }

  /* Reads input and strips newlines */
  printf("\n   Target: ");
  read_line(buffer, MAX_NAME_SIZE);
  strip(buffer, MAX_NAME_SIZE);
  msg->address = strndup(buffer, MAX_NAME_SIZE-1);

  printf("  Message: ");
  read_line(buffer, MAX_MESSAGE_SIZE);
  strip(buffer, MAX_MESSAGE_SIZE);
  msg->text = strndup(buffer, MAX_MESSAGE_SIZE-1);

  /* Queues message and signals the client thread */
  send_queue[n_queued_msgs] = msg;
  n_queued_msgs++;
  sem_post(&queued_msgs);

  /* Frees allocated resources */
  free(buffer);

  return OK;
}



/* Interactive unit thread function. */
void *interactive_unit() {
  char *buffer = NULL;
  char option = '\0';
  int rc;

  /* Waits on barrier for other units to load */
  debug(COLOR_GREEN, "Interactive", "Waiting on barrier for units to load");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    debugerr(COLOR_GREEN, "Interactive", "Error waiting on barrier");
    pthread_exit((void *) E_CANT_WAIT_ON_BARRIER);
  }

  debug(COLOR_GREEN, "Interactive", "Starting");

  /* Allocates memory for user input buffer */
  buffer = malloc(MAX_OPTION_SIZE * sizeof(char));
  if (!buffer) {
    debugerr(COLOR_GREEN, "Interactive", "Error allocating buffer");
    pthread_exit((void *) E_CANT_ALLOC_BUFFER);
  }

  /* Menu loop */
  while (option != '6') {
    print_menu();

    rc = read_line(buffer, MAX_OPTION_SIZE);
    sscanf(buffer, "%c", &option);

    if (rc == OK) {
      if (option == '1') {
        rc = add_contact();
        if (rc == OK) {
          debug(COLOR_GREEN, "Interactive", "Contact added");
        }
        else if (rc == E_CANT_ALLOC_BUFFER) {
          debugerr(COLOR_GREEN, "Interactive", "Couldn't alloc contact buffer");
          pthread_exit((void *) E_CANT_ALLOC_BUFFER);
        } else if (rc == E_CANT_ALLOC_CONTACT) {
          debugerr(COLOR_GREEN, "Interactive", "Couldn't alloc contact struct");
          pthread_exit((void *) E_CANT_ALLOC_CONTACT);
        }
      }
      else if (option == '2') {
        print_contacts();
      }
      else if (option == '3') {
        printf("\nContact removed\n\n");
      }
      else if (option == '4') {
        rc = queue_message();
        if (rc == OK) {
          debug(COLOR_GREEN, "Interactive", "Message queued");
        }
        else if (rc == E_CANT_ALLOC_BUFFER) {
          debugerr(COLOR_GREEN, "Interactive", "Couldn't alloc message buffer");
          pthread_exit((void *) E_CANT_ALLOC_BUFFER);
        }
        else if (rc == E_CANT_ALLOC_MESSAGE) {
          debugerr(COLOR_GREEN, "Interactive", "Couldn't alloc message struct");
          pthread_exit((void *) E_CANT_ALLOC_MESSAGE);
        }
      }
      else if (option == '5') {
        printf("\nMessages read\n\n");
      }
      else if (option == '6') {
        printf("\n  Bye!\n\n");
      }
      else {
        printf("Invalid option\n\n");
      }
    }
  }

  /* Frees allocated resources */
  debug(COLOR_GREEN, "Interactive", "Freeing resources");
  free(buffer);

  /* TODO: mutual exclusion on global variable */
  is_executing = 0;
  sem_post(&queued_msgs);

  /* Waits on barrier for all units to exit together */
  debug(COLOR_GREEN, "Interactive", "Waiting on barrier for units to exit");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    debugerr(COLOR_GREEN, "Interactive", "Error waiting on barrier");
    pthread_exit((void *) E_CANT_WAIT_ON_BARRIER);
  }

  debug(COLOR_GREEN, "Interactive", "Exiting");
  pthread_exit(OK);
}
