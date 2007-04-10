#include "shotgun.h"
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "flags.h"

rstate rubinius_state_new() {
  rstate st;
  st = (rstate)calloc(1, sizeof(struct rubinius_state));
  st->om = object_memory_new();
  st->free_contexts = g_ptr_array_new();
  st->global = (struct rubinius_globals*)calloc(1, sizeof(struct rubinius_globals));
  st->cleanup = g_hash_table_new(NULL, NULL);
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

  g_ptr_array_free(roots, TRUE);
  
  baker_gc_find_lost_souls(state, state->om->gc);
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

  g_ptr_array_free(roots, TRUE); 
}

void state_object_become(STATE, cpu c, OBJECT from, OBJECT to) {
  GPtrArray *roots;
  c->context_cache = 0;
  state->free_contexts->len = 0;
        
  state->current_stack = c->stack_top;
  state->current_sp =    c->sp_ptr;

  roots = _gather_roots(state, c);
  
  object_memory_setup_become(state, state->om, from, to);
  
  /* If from is young, then all the refs are from other young objects
     or the remember set, so we just need to mutate in the young space. */
  if(GC_ZONE(from) == GC_YOUNG_OBJECTS) {
    object_memory_collect(state, state->om, roots);
  } else {
    object_memory_major_collect(state, state->om, roots);
  }
  
  object_memory_clear_become(state, state->om);
  
  memcpy(state->global, roots->pdata, sizeof(struct rubinius_globals));
  cpu_update_roots(state, c, roots, NUM_OF_GLOBALS);

  g_ptr_array_free(roots, TRUE); 
  
}

void state_add_cleanup(STATE, OBJECT cls, state_cleanup_func func) {
  unsigned int cur;
  g_hash_table_insert(state->cleanup, 
      (gpointer)module_get_name(cls),
      (gpointer)func);
            
  cur = (unsigned int)FIXNUM_TO_INT(class_get_instance_flags(cls));
  class_set_instance_flags(cls, I2N(cur | RequiresCleanupFlag));
}

void state_run_cleanup(STATE, OBJECT obj) {
  OBJECT cls;
  state_cleanup_func func;
  
  cls = object_class(state, obj);
  
  func = g_hash_table_lookup(state->cleanup, (gconstpointer)module_get_name(cls));
  if(func) {
    func(state, obj);
  }
}
