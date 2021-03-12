#ifndef log_c
#define log_c

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define BUFF_SIZE        1024*1024

extern FILE * log_file;

int wedis_log(const char *fmt, ...);

#endif