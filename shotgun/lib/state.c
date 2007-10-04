#include "shotgun.h"
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "flags.h"
#include <sys/time.h>
#include "cleanup_hash.h"
#include "config_hash.h"

rstate rubinius_state_new() {
  rstate st;
  st = (rstate)calloc(1, sizeof(struct rubinius_state));
  st->om = object_memory_new();
  st->free_contexts = ptr_array_new(8);
  st->global = (struct rubinius_globals*)calloc(1, sizeof(struct rubinius_globals));
  st->cleanup = ht_cleanup_create(11);
  st->config = ht_config_create(11);
  return st;
}

static ptr_array _gather_roots(STATE, cpu c) {
  ptr_array roots;
  roots = ptr_array_new(NUM_OF_GLOBALS + 100);
  
  memcpy(roots->array, state->global, sizeof(struct rubinius_globals));
  roots->length = NUM_OF_GLOBALS;
  
  cpu_add_roots(state, c, roots);
  /* truncate the free_context list since we don't care about them
     after we've collected anyway */
  return roots; 
}

void cpu_sampler_suspend(STATE);
void cpu_sampler_resume(STATE);
void cpu_hard_cache(STATE, cpu c);

void state_collect(STATE, cpu c) {
  ptr_array roots;
  int stats = state->gc_stats;
  struct timeval start, fin;
  
  if(stats) {
    gettimeofday(&start, NULL);
  }
  
  c->context_cache = 0;
  ptr_array_clear(state->free_contexts);
  
  cpu_flush_ip(c);
  cpu_flush_sp(c);
  
  state->current_stack = c->stack_top;
  state->current_sp =    c->sp_ptr;
  
  /* HACK: external_ivars needs to be moved out of being a generic
      global and being a special case one so that it's references
      can't keep objects alive. */
  
  cpu_sampler_suspend(state);
  object_memory_formalize_contexts(state, state->om);
  roots = _gather_roots(state, c);
  object_memory_collect(state, state->om, roots);
  memcpy(state->global, roots->array, sizeof(struct rubinius_globals));
  cpu_update_roots(state, c, roots, NUM_OF_GLOBALS);

  ptr_array_free(roots);
  
  baker_gc_find_lost_souls(state, state->om->gc);
  cpu_sampler_resume(state);
  
  if(stats) {
    double elapse;
    gettimeofday(&fin, NULL);
    elapse =  (fin.tv_sec - start.tv_sec);
    elapse += (((double)fin.tv_usec - start.tv_usec) / 1000000);
    printf("[GC Y %f secs, %dK total, %3dK used, %4d tenured, %d]\n", 
      elapse,
      (state->om->gc->current->size / 1024),
      (int)((state->om->gc->current->current - state->om->gc->current->address) / 1024),
      state->om->last_tenured,
      state->om->gc->num_collection
    );
  }
  
  cpu_hard_cache(state, c);
  cpu_cache_sp(c);  
}


void state_major_collect(STATE, cpu c) {
  ptr_array roots;
  int stats = state->gc_stats;
  struct timeval start, fin;
  
  state_collect(state, c);
    
  if(stats) {
    gettimeofday(&start, NULL);
  }
  
  cpu_flush_ip(c);
  cpu_flush_sp(c);
  
  c->context_cache = 0;
  ptr_array_clear(state->free_contexts);
  
  /* HACK: external_ivars needs to be moved out of being a generic
      global and being a special case one so that it's references
      can't keep objects alive. */
      
  state->current_stack = c->stack_top;
  state->current_sp =    c->sp_ptr;

  cpu_sampler_suspend(state);
  roots = _gather_roots(state, c);
  object_memory_major_collect(state, state->om, roots);
  memcpy(state->global, roots->array, sizeof(struct rubinius_globals));
  cpu_update_roots(state, c, roots, NUM_OF_GLOBALS);

  ptr_array_free(roots);
  cpu_sampler_suspend(state);
  
  if(stats) {
    double elapse;
    gettimeofday(&fin, NULL);
    elapse =  (fin.tv_sec - start.tv_sec);
    elapse += (((double)fin.tv_usec - start.tv_usec) / 1000000);
    
    printf("[GC M %f secs, %d freed, %d total, %d segments, %6dK total]\n", 
      elapse,
      state->om->ms->last_freed, state->om->ms->last_marked,
      state->om->ms->num_chunks,
      state->om->ms->allocated_bytes / 1024
      );
  }
  
  cpu_hard_cache(state, c);  
  cpu_cache_sp(c);
}

void state_object_become(STATE, cpu c, OBJECT from, OBJECT to) {
  ptr_array roots;
  c->context_cache = 0;
  ptr_array_clear(state->free_contexts);
        
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
  
  memcpy(state->global, roots->array, sizeof(struct rubinius_globals));
  cpu_update_roots(state, c, roots, NUM_OF_GLOBALS);

  ptr_array_free(roots);
  
}

void state_add_cleanup(STATE, OBJECT cls, state_cleanup_func func) {
  unsigned int cur;
  
  ht_cleanup_insert(state->cleanup, module_get_name(cls), func);
  
  // printf("Registered cleanup for %p\n", module_get_name(cls));
  cur = (unsigned int)FIXNUM_TO_INT(class_get_instance_flags(cls));
  class_set_instance_flags(cls, I2N(cur | RequiresCleanupFlag));
}

void state_run_cleanup(STATE, OBJECT obj, OBJECT cls) {
  state_cleanup_func func;
    
  if(!REFERENCE_P(cls)) return;
  
  // printf("Cleaning up %p (%s, %p)\n", obj, _inspect(cls), module_get_name(cls));
  func = ht_cleanup_search(state->cleanup, module_get_name(cls));
  if(func) {
    func(state, obj);
  }
}
