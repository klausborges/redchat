#include <stdio.h>
#include "../include/utils.h"
#include "../include/redchat.h"

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
