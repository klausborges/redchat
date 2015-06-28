#include <stdio.h>
#include <string.h>
#include "../include/utils.h"
#include "../include/redchat.h"

int read_line(char *buffer, size_t size) {
  char c;
  int extra = 0;

  if (fgets(buffer, size, stdin) == NULL) {
    return E_INPUT_EMPTY;
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

void debug(char *color, char *name, char *str) {
  if (DEBUG) {
    printf("[%s%s%s] %s\n", color, name, COLOR_RESET, str);
  }
}

void debugerr(char *color, char *name, char *str) {
  if (DEBUG) {
    printf("[%s%s%s] [%s!%s] %s\n", color, name, COLOR_RESET, COLOR_RED,
        COLOR_RESET, str);
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
