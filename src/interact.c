#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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
  printf("5. Send group message\n");
  printf("6. Read all messages (%d)\n", n_msgs);
  printf("7. Read unread messages (%d)\n", n_unread_msgs);
  printf("8. Exit\n");
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

  /* Sets send time and read status of the message */
  msg->time_sent = time(NULL);
  msg->read = FALSE;


  /* Queues message and signals the client thread */
  send_queue[n_queued_msgs] = msg;
  n_queued_msgs++;
  sem_post(&queued_msgs);

  /* Frees allocated resources */
  free(buffer);

  return OK;
}



/* Removes the nth contact from the contact list. */
static int remove_contact() {
  char *buffer = NULL;

  /* Allocates memory for buffer */
  buffer = (char *) malloc(MAX_BUFFER_SIZE * sizeof(char));
  if (!buffer) {
    return E_CANT_ALLOC_BUFFER;
  }

  /* Reads input and strips newlines */
  printf("\n  Number (n): ");
  read_line(buffer, MAX_NAME_SIZE);
  strip(buffer, MAX_NAME_SIZE);

  /* Removes contact from contact list */


  /* Frees allocated resources */
  free(buffer);

  return OK;
}



/* Lists all received messages and sets all unread to read. */
static void print_all_messages() {
  int i;

  if (n_msgs == 0) {
    printf("\n  No messages to show!\n\n");
  }
  else {
    char buffer[10];
    struct tm *time_received;

    printf("\n  %*s %*s %s\n", MAX_TIME_SIZE, "Time", MAX_NAME_SIZE,
        "From", "Message");
    for (i = 0; i < n_msgs; i++) {
      time_received = localtime(&messages[i]->time_received);
      strftime(buffer, 10, "%T", time_received);
      printf("  %*s %*s %s\n",
          MAX_TIME_SIZE, buffer,
          MAX_NAME_SIZE, messages[i]->address,
          messages[i]->text);

      /* Marks message as read */
      if (messages[i]->read == FALSE) {
        messages[i]->read = TRUE;
        n_unread_msgs--;
      }
    }

    /* Frees allocated resources */

    printf("\n");
  }
}

/* Lists unread received messages and sets them to read. */
static void print_unread_messages() {
  int i;

  if (n_unread_msgs == 0) {
    printf("\n  No unread messages to show!\n\n");
  }
  else {
    char buffer[10];
    struct tm *time_received;

    printf("\n  %*s %*s %s\n", MAX_TIME_SIZE, "Time", MAX_NAME_SIZE,
        "From", "Message");
    for (i = 0; i < n_msgs; i++) {
      if (messages[i]->read == FALSE) {
        time_received = localtime(&messages[i]->time_received);
        strftime(buffer, 10, "%T", time_received);
        printf("  %*s %*s %s\n",
            MAX_TIME_SIZE, buffer,
            MAX_NAME_SIZE, messages[i]->address,
            messages[i]->text);

        /* Marks message as read */
        messages[i]->read = TRUE;
        n_unread_msgs--;
      }
    }
    printf("\n");
  }
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
  while (option != '8') {
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
        rc = remove_contact();
        if (rc == OK) {
          debug(COLOR_GREEN, "Interactive", "Contact removed");
        }
        else if (rc == E_CANT_ALLOC_BUFFER) {
          debugerr(COLOR_GREEN, "Interactive", "Couldn't alloc removal buffer");
          pthread_exit((void *) E_CANT_ALLOC_BUFFER);
        }
        else if (rc == E_INVALID_CONTACT_N) {
          debugerr(COLOR_GREEN, "Interactive", "Invalid contact number");
        }
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
        /* TODO */
      }
      else if (option == '6') {
        print_all_messages();
      }
      else if (option == '7') {
        print_unread_messages();
      }
      else if (option == '8') {
        /* TODO: Signal other threads to free up resources and exit */
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
