/**
 * EV3 HRP app source
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define LOG_FILE "ev3rt/etc/rover.log"

FILE * log_file = NULL;
void log_printf(const char * restrict format, ...) {
  if (NULL == log_file) log_file = fopen(LOG_FILE, "w");
  va_list args;
  va_start(args, format);
  vfprintf(log_file, format, args);
  va_end(args);
  fflush(log_file);
}
