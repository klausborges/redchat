#ifndef UTILS_H
#define UTILS_H

#define TRUE  1
#define FALSE 0

#define DEBUG           TRUE
#define DEBUG_TIMESTAMP FALSE
#define DEBUG_COLORS    TRUE

#define COLOR_BLACK   "\e[30m"
#define COLOR_RED     "\e[31m"
#define COLOR_GREEN   "\e[32m"
#define COLOR_YELLOW  "\e[33m"
#define COLOR_BLUE    "\e[34m"
#define COLOR_MAGENTA "\e[35m"
#define COLOR_CYAN    "\e[36m"
#define COLOR_RESET   "\e[39m"

/* Prints a colorized debug message on stdout. */
void printd(char *, char *);

#endif
