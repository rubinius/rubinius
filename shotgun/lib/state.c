#include "shotgun.h"
#include <stdlib.h>

rstate rubinius_state_new() {
  rstate st;
  st = (rstate)malloc(sizeof(struct rubinius_state));
  st->om = object_memory_new();
  st->free_contexts = g_ptr_array_new();
  st->global = (struct rubinius_globals*)malloc(sizeof(struct rubinius_globals));
  return st;
}
