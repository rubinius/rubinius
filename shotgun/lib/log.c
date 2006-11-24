#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static int debug_enabled = 0;

void debug_enable() {
  debug_enabled = 1;
}

void debug_disable() {
  debug_enabled = 0;
}

void log_debug(const char *fmt, ...) {
  if(!debug_enabled) return;
  printf("DEBUG: ");
  va_list ap;
  va_start(ap, fmt);
  (void) vprintf(fmt, ap);
  va_end(ap);
  return;
}