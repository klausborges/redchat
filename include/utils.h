#ifndef UTILS_H
#define UTILS_H

/* Boolean value macros. */
#define TRUE  1
#define FALSE 0

/* Bash color formatting codes. */
#define COLOR_BLACK   "\e[30m"
#define COLOR_RED     "\e[31m"
#define COLOR_GREEN   "\e[32m"
#define COLOR_YELLOW  "\e[33m"
#define COLOR_BLUE    "\e[34m"
#define COLOR_MAGENTA "\e[35m"
#define COLOR_CYAN    "\e[36m"
#define COLOR_RESET   "\e[39m"

/* Prints a colorized debug message on stdout. */
void debug(char *, char *, char *);

/* Prints a colorized debug error message on stdout. */
void debugerr(char *, char *, char *);

/* Strips newlines from strings. */
void strip(char *, int);

#endif
