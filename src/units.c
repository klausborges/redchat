#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/units.h"
#include "../include/utils.h"

static int read_line(char *buffer, size_t size) {
  char c;
  int extra;

  if (fgets(buffer, size, stdin) == NULL) {
    return E_NO_INPUT;
  }

  if (buffer[strlen(buffer)-1] != '\n') {
    while (((c = getchar()) != '\n') && (c != EOF)) {
      extra = 1;
    }
    return (extra == 1) ? E_INPUT_TOO_LONG : OK;
  }

  buffer[strlen(buffer)-1] = '\0';
  return OK;
}

static void print_menu() {
  printf("1. Add contact IP\n");
  printf("2. List contacts\n");
  printf("3. Remove contact\n");
  printf("4. Send message\n");
  printf("5. Read messages\n");
  printf("Option> ");
}

static int queue_message() {
  char *buffer = NULL;
  char *target = NULL;
  char *msg = NULL;

  buffer = (char *) malloc(MAX_BUFFER_SIZE * sizeof(char));
  if (!buffer) {
    fprintf(stderr, "Error allocating memory for send_message() buffer\n");
    return E_BUF_MALLOC;
  }

  printf("\nTarget: ");
  read_line(buffer, MAX_NAME_SIZE);
  target = strndup(buffer, MAX_NAME_SIZE-1);

  printf("Message: ");
  read_line(buffer, MAX_MESSAGE_SIZE);
  msg = strndup(buffer, MAX_MESSAGE_SIZE-1);

  strip(target, MAX_NAME_SIZE);
  strip(msg, MAX_MESSAGE_SIZE);

  printf("\n @%s %s\n", target, msg);

  free(buffer);

  return 0;
}


void *interactive_unit() {
  char *buffer = NULL;
  char option = '\0';
  int rc;

  printd("Interactive", "Starting");

  buffer = malloc(MAX_OPTION_SIZE * sizeof(char));
  if (!buffer) {
    fprintf(stderr, "Error allocating memory for interactive_unit() buffer\n");
    return (void *) E_BUF_MALLOC;
  }

  while (option != '6') {
    print_menu();

    rc = read_line(buffer, MAX_OPTION_SIZE);
    sscanf(buffer, "%c", &option);

    if (rc == OK) {
      if (option == '1') {
        printf("\nContact added\n\n");
      }
      else if (option == '2') {
        printf("\nContacts listed\n\n");
      }
      else if (option == '3') {
        printf("\nContact removed\n\n");
      }
      else if (option == '4') {
        queue_message();
        printf("\nMessage sent\n\n");
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

  printd("Interactive", "Exiting");
  return OK;
}

void *client_unit() {
  printd("Client", "Client unit starting");

  printd("Client", "Client unit exiting");
  return OK;
}

void *server_unit() {
  printd("Server", "Server unit starting");

  printd("Server", "Server unit exiting");
  return OK;
}
