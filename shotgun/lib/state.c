#include "shotgun.h"
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

rstate rubinius_state_new() {
  rstate st;
  st = (rstate)calloc(1, sizeof(struct rubinius_state));
  st->om = object_memory_new();
  st->free_contexts = g_ptr_array_new();
  st->global = (struct rubinius_globals*)calloc(1, sizeof(struct rubinius_globals));
  return st;
}

static GPtrArray *_gather_roots(STATE, cpu c) {
  GPtrArray *roots;
  roots = g_ptr_array_sized_new(NUM_OF_GLOBALS + 100);
  
  memcpy(roots->pdata, state->global, sizeof(struct rubinius_globals));
  roots->len = NUM_OF_GLOBALS;
  
  cpu_add_roots(state, c, roots);
  /* truncate the free_context list since we don't care about them
     after we've collected anyway */
  return roots; 
}

void state_collect(STATE, cpu c) {
  GPtrArray *roots;
  c->context_cache = 0;
  state->free_contexts->len = 0;
  
  state->current_stack = c->stack_top;
  state->current_sp =    c->sp_ptr;
  
  /* HACK: external_ivars needs to be moved out of being a generic
      global and being a special case one so that it's references
      can't keep objects alive. */

  roots = _gather_roots(state, c);
  object_memory_collect(state, state->om, roots);
  memcpy(state->global, roots->pdata, sizeof(struct rubinius_globals));
  cpu_update_roots(state, c, roots, NUM_OF_GLOBALS);

  g_ptr_array_free(roots, 0);
}

void state_major_collect(STATE, cpu c) {
  GPtrArray *roots;
  c->context_cache = 0;
  state->free_contexts->len = 0;
  
  /* HACK: external_ivars needs to be moved out of being a generic
      global and being a special case one so that it's references
      can't keep objects alive. */
      
  state->current_stack = c->stack_top;
  state->current_sp =    c->sp_ptr;

  roots = _gather_roots(state, c);
  object_memory_major_collect(state, state->om, roots);
  memcpy(state->global, roots->pdata, sizeof(struct rubinius_globals));
  cpu_update_roots(state, c, roots, NUM_OF_GLOBALS);

  g_ptr_array_free(roots, 0); 
}


