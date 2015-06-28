#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../include/redchat.h"
#include "../include/utils.h"

static void print_menu() {
  printf("1. Add contact IP\n");
  printf("2. List contacts\n");
  printf("3. Remove contact\n");
  printf("4. Send message\n");
  printf("5. Read messages (%d)\n", n_msgs);
  printf("6. Exit\n");
  printf("Option> ");
}

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

static int add_contact() {
  if (n_contacts == MAX_N_CONTACTS) {
    printf("\n  Contact list is full!\n\n");
    return E_CONTACT_LIST_FULL;
  }
  else {
    printf("Adding contact");
    char *buffer = NULL;
    struct contact *new_contact = NULL;

    buffer = (char *) malloc(MAX_ADDRESS_SIZE * sizeof(char));
    if (!buffer) {
      debugerr(COLOR_GREEN, "Interactive", "Error allocating memory for add_contact buffer\n");
      return E_CANT_ALLOC_BUFFER;
    }

    new_contact = (struct contact *) malloc(sizeof(struct contact));
    if (!new_contact) {
      debugerr(COLOR_GREEN, "Interactive", "Couldn't allocate memory for contact struct");
      return E_CANT_ALLOC_CONTACT;
    }

    printf("\n     IP: ");
    read_line(buffer, MAX_ADDRESS_SIZE);
    strip(buffer, MAX_ADDRESS_SIZE);
    new_contact->address = strndup(buffer, MAX_ADDRESS_SIZE-1);

    printf("  Alias: ");
    read_line(buffer, MAX_NAME_SIZE);
    strip(buffer, MAX_NAME_SIZE);
    new_contact->alias = strndup(buffer, MAX_NAME_SIZE-1);

    new_contact->status = STATUS_UNKNOWN;

    /* Adds new contact to the end of the linked list */
    contacts[n_contacts] = new_contact;
    n_contacts++;

    free(buffer);

    printf("\n");
    return OK;
  }
}

static int queue_message() {
  char *buffer = NULL;
  struct message *msg = NULL;

  buffer = (char *) malloc(MAX_BUFFER_SIZE * sizeof(char));
  if (!buffer) {
    debugerr(COLOR_GREEN, "Interactive", "Error allocating memory for send_message buffer\n");
    return E_CANT_ALLOC_BUFFER;
  }

  msg = (struct message *) malloc(sizeof(struct message));
  if (!msg) {
    debugerr(COLOR_GREEN, "Interactive", "Couldn't allocate memory for message struct");
    return E_CANT_ALLOC_MESSAGE;
  }

  printf("\nTarget: ");
  read_line(buffer, MAX_NAME_SIZE);
  strip(buffer, MAX_NAME_SIZE);
  msg->address = strndup(buffer, MAX_NAME_SIZE-1);

  printf("Message: ");
  read_line(buffer, MAX_MESSAGE_SIZE);
  strip(buffer, MAX_MESSAGE_SIZE);
  msg->text = strndup(buffer, MAX_MESSAGE_SIZE-1);


  printf("\n @%s %s\n", msg->address, msg->text);

  /* Queue message */
  /* TODO: mutex */
  send_queue[n_queued_msgs] = msg;
  n_queued_msgs++;
  sem_post(&queued_msgs);

  free(buffer);

  return 0;
}

void *interactive_unit() {
  char *buffer = NULL;
  char option = '\0';
  int rc;

  debug(COLOR_GREEN, "Interactive", "Waiting on barrier for all threads to load");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    fprintf(stderr, "Error waiting on barrier\n");
    return (void *) E_CANT_WAIT_ON_BARRIER;
  }

  debug(COLOR_GREEN, "Interactive", "Starting");

  buffer = malloc(MAX_OPTION_SIZE * sizeof(char));
  if (!buffer) {
    fprintf(stderr, "Error allocating memory for interactive_unit() buffer\n");
    return (void *) E_CANT_ALLOC_BUFFER;
  }

  while (option != '6') {
    print_menu();

    rc = read_line(buffer, MAX_OPTION_SIZE);
    sscanf(buffer, "%c", &option);

    if (rc == OK) {
      if (option == '1') {
        add_contact();
      }
      else if (option == '2') {
        print_contacts();
      }
      else if (option == '3') {
        printf("\nContact removed\n\n");
      }
      else if (option == '4') {
        queue_message();
      }
      else if (option == '5') {
        printf("\nMessages read\n\n");
      }
      else if (option == '6') {
        printf("\nBye!\n\n");
      }
      else {
        printf("Invalid option\n\n");
      }
    }
  }

  free(buffer);

  /* MUTEX HERE */
  is_executing = 0;
  sem_post(&queued_msgs);

  debug(COLOR_GREEN, "Interactive", "Waiting on barrier");
  rc = pthread_barrier_wait(&all_done);
  if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    fprintf(stderr, "Error waiting on barrier\n");
    pthread_exit((void *) E_CANT_WAIT_ON_BARRIER);
  }

  debug(COLOR_GREEN, "Interactive", "Exiting");
  pthread_exit(OK);
}
