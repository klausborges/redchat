#include <stdio.h>
#include "../include/units.h"
#include "../include/utils.h"

void *interactive_unit() {
  int i;
  printd("Interactive", "Starting\n");

  for (i = 0; i < 5; i++) {
    printf("Menu\n");
  }

  printd("Interactive", "Exiting\n");
  return 0;
}

void *client_unit() {
  printd("Client", "Client unit starting\n");

  printd("Client", "Client unit exiting\n");
  return 0;
}

void *server_unit() {
  printd("Server", "Server unit starting\n");

  printd("Server", "Server unit exiting\n");
  return 0;
}
