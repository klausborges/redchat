#include <stdio.h>
#include "../include/utils.h"

void printd(char *unit_name, char *debug_str) {
  if (DEBUG) {
    printf("\e[31m%s --- %s", unit_name, debug_str);
  }
}
