#include <stdio.h>
#include "../include/utils.h"

void printd(char *unit_name, char *debug_str) {
  if (DEBUG) {
    printf("[\e[31m%s\e[0m] %s\e[0m\n", unit_name, debug_str);
  }
}

void strip(char *str, int len) {
  int i;

  for (i = 0; i < len; i++) {
    if (str[i] == '\n') {
      str[i] = '\0';
      break;
    }
  }
}
