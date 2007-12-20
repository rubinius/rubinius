#include "shotgun.h"
#include <stdlib.h>
#include <string.h>
#include "heap.h"
#include "cpu.h"
#include "methctx.h"
#include "bytearray.h"
#include "baker.h"
#include "tuple.h"

#define DEFAULT_TENURE_AGE 6

baker_gc baker_gc_new(int size) {
  baker_gc g;
  g = (baker_gc)calloc(1, sizeof(struct baker_gc_struct));
  g->remember_set = ptr_array_new(8);
  g->seen_weak_refs = ptr_array_new(8);
  
  g->space_a = heap_new(size);
  g->space_b = heap_new(size);
  g->current = g->space_a;
  g->next =    g->space_b;
  g->used =    0;
  g->tenure_age = DEFAULT_TENURE_AGE;
  g->become_from = Qnil;
  g->become_to = Qnil;
  return g;
}

void baker_gc_describe(baker_gc g) {
  printf("Size:    %x (%d)\n", g->current->size, g->current->size);
  printf("Used:    %d\n", g->used);
  printf("Current: %p => %p\n", (void*)g->current->address, 
      (void*)g->current->last);
  printf("Next:    %p => %p\n", (void*)g->next->address, (void*)g->next->last);
  printf("RS Size: %zd\n", ptr_array_length(g->remember_set));
}

int baker_gc_enlarge_next(baker_gc g, int sz) {
  rheap h;
  h = heap_new(sz);
  return baker_gc_set_next(g, h);
}

int baker_gc_set_next(baker_gc g, rheap h) {
  if(g->next == g->space_a) {
    g->space_a = h;
  } else {
    g->space_b = h;
  }
  g->next = h;
  return TRUE;
}

address baker_gc_start_address(baker_gc g) {
  return g->current->address;
}

int baker_gc_used(baker_gc g) {
  return g->used;
}

int baker_gc_swap(baker_gc g) {
  rheap tmp;
  
  tmp = g->current;
  g->current = g->next;
  g->next = tmp;
  
  g->last_start = (char*)tmp->address;
  g->last_end =   (char*)tmp->current;
  
  heap_reset(tmp);
  /* Reset used to the what the current has used. */
  g->used = (uintptr_t)g->current->current - (uintptr_t)g->current->address;
  return TRUE;
}

int baker_gc_destroy(baker_gc g) {
  heap_deallocate(g->space_a);
  heap_deallocate(g->space_b);
  return TRUE;
}

void baker_gc_set_forwarding_address(OBJECT obj, OBJECT dest) {
  SET_FORWARDED(obj);
  obj->klass = dest;
}

inline int baker_gc_forwarded_p(OBJECT obj) {
  return FORWARDED_P(obj);
}

OBJECT baker_gc_forwarded_object(OBJECT obj) {
  OBJECT out = obj->klass;
  CHECK_PTR(out);
  return out;
}

#define baker_gc_maybe_mutate(st, g, iobj) ({     \
  OBJECT ret;                                 \
  if(baker_gc_forwarded_p(iobj)) {            \
    ret = baker_gc_forwarded_object(iobj);    \
  } else if(baker_gc_contains_p(g, iobj) || heap_contains_p(state->om->contexts, iobj)) {   \
    assert(((OBJECT)iobj)->klass); \
    ret = baker_gc_mutate_object(st, g, iobj);    \
  } else {                                    \
    ret = iobj;                               \
  }                                           \
  ret; })

/*
static inline OBJECT baker_gc_maybe_mutate(baker_gc g, OBJECT iobj) {
  OBJECT ret;

  if(baker_gc_forwarded_p(iobj)) {
    ret = baker_gc_forwarded_object(iobj);
  } else if(baker_gc_contains_p(g, iobj)) {
    ret = baker_gc_mutate_object(g, iobj);
  } else {
    ret = iobj;
  }
  
  // assert(baker_gc_contains_spill_p(g, ret));
    
  CHECK_PTR(ret);
  
  return ret;
}
*/

int _object_stores_bytes(OBJECT self);
static int depth = 0;
static ptr_array _track_refs = NULL;

void track_ref(OBJECT obj) {
  if(!_track_refs) {
    _track_refs = ptr_array_new(8);
  }
  ptr_array_append(_track_refs, (xpointer)obj);
}

void untrack_ref(OBJECT obj) {
  if(!_track_refs) return;
  ptr_array_remove_fast(_track_refs, (xpointer)obj);
}

static inline void _mutate_references(STATE, baker_gc g, OBJECT iobj) {
  OBJECT cls, tmp, mut;
  int i, fields;
  
#if 0
  if(_track_refs) {
    if (ptr_array_contains(_track_refs, (xpointer)iobj))
        printf("Found2 %p!\n", (void*)iobj);
  }
#endif
  
  //printf("%d: Mutating class of %p\n", ++depth, iobj);
 
  /* CLASS_OBJECT() gets the object directly in the klass field
   * of iobj, which is very likely it's metaclass, not it's 
   * formal class. FYI. */
  cls = CLASS_OBJECT(iobj);
  if(REFERENCE_P(cls)) {
    cls = baker_gc_maybe_mutate(state, g, cls);
  }
  
  SET_CLASS(iobj, cls);
  
  xassert(!FORWARDED_P(iobj));
  
  if(iobj->RefsAreWeak) {
    // printf("%p has weak refs.\n", (void*)iobj);
    ptr_array_append(g->seen_weak_refs, (xpointer)iobj);
    depth--;
    return;
  }
    
  //printf("%d: Mutating references of %p\n", depth, iobj);
  
  if(!_object_stores_bytes(iobj)) {
    fields = NUM_FIELDS(iobj);
    for(i = 0; i < fields; i++) {
      tmp = NTH_FIELD(iobj, i);
      if(!REFERENCE_P(tmp)) continue;
    
      mut = baker_gc_maybe_mutate(state, g, tmp);
      rbs_set_field(g->om, iobj, i, mut);
    }
  } else {
#define fc_mutate(field) if(fc->field && REFERENCE_P(fc->field)) SET_STRUCT_FIELD(iobj, fc->field, baker_gc_maybe_mutate(state, g, fc->field))
    if(methctx_is_context_p(state, iobj)) {
      struct fast_context *fc = FASTCTX(iobj);
      fc_mutate(sender);
      fc_mutate(block);
      fc_mutate(method);
      fc_mutate(literals);
      fc_mutate(self);
      fc_mutate(locals);
      fc_mutate(method_module);
      fc_mutate(name);

      if(!NIL_P(fc->method)) {
        /* We cache the bytecode in a char*, so adjust it. 
           We mutate the data first so we cache the newest address. */
        OBJECT ba;
        ba = cmethod_get_compiled(fc->method);
        ba = baker_gc_maybe_mutate(state, g, ba);
      
        fc->data = BYTEARRAY_ADDRESS(ba);
      }
    } else if(iobj->obj_type == TaskType) {
      struct cpu_task *fc = (struct cpu_task*)BYTES_OF(iobj);
      /* Only mark up non-active tasks, because the active
         one is handled else where. */
      if(!fc->active) {
        if(REFERENCE_P(fc->active_context)) {
          assert(fc->active_context->obj_type == MContextType ||
                fc->active_context->obj_type == BContextType);
        }
        
        if(REFERENCE_P(fc->home_context)) {
          assert(fc->home_context->obj_type == MContextType ||
                fc->home_context->obj_type == BContextType);
        }
        
        fc_mutate(exception);
        fc_mutate(new_class_of);
        fc_mutate(enclosing_class);
        fc_mutate(exceptions);
        fc_mutate(active_context);
        fc_mutate(home_context);
        fc_mutate(main);  
        fc_mutate(outstanding);
        fc_mutate(debug_channel);
        fc_mutate(control_channel);
        fc_mutate(current_scope);
        OBJECT *sp;

        // printf("marking %p, %p (%p, %d)\n", iobj, fc->stack_top, fc->sp_ptr, fc->sp_ptr - fc->stack_top);
        sp = fc->stack_top;
        
        assert(fc->sp_ptr >= sp);
        
        while(sp <= fc->sp_ptr) {
          if(*sp && REFERENCE_P(*sp)) {
            *sp = baker_gc_maybe_mutate(state, g, *sp);
          }
          sp++;
        }
      
        int i;
        for(i = 0; i < ptr_array_length(fc->paths); i++) {
          ptr_array_set_index(fc->paths, i,
            (xpointer)baker_gc_maybe_mutate(state, g, (OBJECT)ptr_array_get_index(fc->paths, i)));
        }
      }
    }
    
#undef fc_mutate
  }
  
  depth--;
}

void baker_gc_mutate_context(STATE, baker_gc g, OBJECT iobj, int shifted, int top) {
  #define fc_mutate(field) if(fc->field && REFERENCE_P(fc->field)) fc->field = baker_gc_mutate_from(state, g, fc->field)
  
  struct fast_context *fc = FASTCTX(iobj);
  OBJECT old_sender;

  old_sender = fc->sender;
  
  if(REFERENCE_P(fc->sender)) {
    /* This is the top most stack context, handle it differently */
    if(top) {
      if(shifted) {
        xassert(om_in_heap(state->om, fc->sender) || om_context_referenced_p(state->om, fc->sender));
        xassert(fc->sender != Qnil);
      } else {
        xassert(om_in_heap(state->om, fc->sender));
      }
      fc_mutate(sender);
    } else {
      /*
      if(shifted) {
        old_sender = fc->sender;
        xassert(om_on_stack(state->om, old_sender));
        fc->sender = old_sender->klass;
        xassert(NIL_P(fc->sender) || fc->sender == om_stack_sender(iobj));
      } else {
        xassert(om_on_stack(state->om, fc->sender));
      }
      */
    }
  }
  
  if(top) {
    assert(NIL_P(fc->sender) || fc->sender->obj_type == MContextType || fc->sender->obj_type == BContextType);
  }
  
  
  
  fc_mutate(method);
  fc_mutate(block);
  fc_mutate(literals);
  fc_mutate(self);
  if(!NIL_P(fc->locals) && fc->locals->gc_zone == 0) {
    int i, fields;
    OBJECT mut, tmp;
    
    fc->locals = object_memory_context_locals(iobj);
    fields = NUM_FIELDS(fc->locals);
    
    for(i = 0; i < fields; i++) {
      tmp = NTH_FIELD(fc->locals, i);
      if(!REFERENCE_P(tmp)) continue;
    
      mut = baker_gc_mutate_from(state, g, tmp);
      fast_unsafe_set(fc->locals, i, mut);
    }
  } else {
    fc_mutate(locals);
  }
  fc_mutate(method_module);
  fc_mutate(name);

  if(!NIL_P(fc->method)) {
    /* We cache the bytecode in a char*, so adjust it. 
       We mutate the data first so we cache the newest address. */
    OBJECT ba;
    ba = cmethod_get_compiled(fc->method);
    ba = baker_gc_mutate_from(state, g, ba);

    fc->data = BYTEARRAY_ADDRESS(ba);
  }
#undef fc_mutate
}

static int saved_contexts;

OBJECT baker_gc_mutate_object(STATE, baker_gc g, OBJECT obj) {
  OBJECT dest;
  if(obj == g->become_from) {
    return baker_gc_maybe_mutate(state, g, g->become_to);
  }
  
  if(heap_contains_p(state->om->contexts, obj)) {
    saved_contexts++;
  }

#if 0
  if(_track_refs) {
    if (ptr_array_contains(_track_refs, (xpointer)obj))
      printf("Found %p!\n", (void*)obj);
  }
#endif
  
  if((AGE(obj) == g->tenure_age)) {
    // int age = AGE(obj);
    xassert(obj->klass != state->global->fastctx);
    CLEAR_AGE(obj);
    /*
    if(CLASS_OBJECT(obj) == state->global->cmethod) {
      printf("Tenuring %p (%s / %d)\n", obj, _inspect(cmethod_get_name(obj)), NUM_FIELDS(obj));
    }
    */
    dest = (*g->tenure)(g->tenure_data, obj);
    baker_gc_set_forwarding_address(obj, dest);
    //printf("Tenuring object %p to %p, age %d (%d).\n", obj, dest, age, g->tenure_now);
    _mutate_references(state, g, dest);
  } else {
    if(heap_enough_fields_p(g->next, NUM_FIELDS(obj))) {
      xassert(obj->klass != Qnil);
      dest = heap_copy_object(g->next, obj);
      baker_gc_set_forwarding_address(obj, dest);
      if(!obj->ForeverYoung) INCREMENT_AGE(dest);
      if(obj->obj_type == WrapsStructType) MARK_WRAPPED_STRUCT(obj);
    } else {
      CLEAR_AGE(obj);
      dest = (*g->tenure)(g->tenure_data, obj);
      baker_gc_set_forwarding_address(obj, dest);
      _mutate_references(state, g, dest);
    }
  }

#if 0
  if(_track_refs) {
    if (ptr_array_contains(_track_refs, (xpointer) dest))
        printf("Found3 %p!\n", (void*)dest);
  }
#endif

  return dest;
}

int baker_gc_contains_p(baker_gc g, OBJECT obj) {
  return heap_contains_p(g->current, obj);
}

int baker_gc_contains_spill_p(baker_gc g, OBJECT obj) {
  return heap_contains_p(g->next, obj) || heap_contains_p(g->current, obj);
}

OBJECT baker_gc_mutate_from(STATE, baker_gc g, OBJECT orig) {
  OBJECT ret, iobj;
  int count = 0;
    
  ret = baker_gc_maybe_mutate(state, g, orig);
  
  iobj = ret;
  
  while(iobj) {
    _mutate_references(state, g, iobj);
    iobj = heap_next_unscanned(g->next);
    count++;
  }
  
  return ret;
}

void baker_gc_mutate_rest(STATE, baker_gc g) {
  OBJECT iobj;

  iobj = heap_next_unscanned(g->next);

  while(iobj) {
    _mutate_references(state, g, iobj);
    iobj = heap_next_unscanned(g->next);
  }
}


int baker_gc_collect(STATE, baker_gc g, ptr_array roots) {
  int i, sz;
  OBJECT tmp, root;
  struct method_cache *end, *ent;
  ptr_array rs;
  
  saved_contexts = 0;
  
  g->num_collection++;
  
  /* empty it out. */
  ptr_array_clear(g->seen_weak_refs);
  
  // printf("Running garbage collector...\n");
  
  
  sz = ptr_array_length(roots);
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(ptr_array_get_index(roots, i));
    // printf("Collecting from root %d\n", i);
    // printf("%p => %p\n", g->current->address, g->next->address);
    // printf("Start at RS (%d): %p\n", i, root);
    if(!REFERENCE2_P(root)) { continue; }
    tmp = baker_gc_mutate_from(state, g, root);
    ptr_array_set_index(roots, i, (xpointer) tmp);
  }
  
  /* To maintain the remember set, we setup a totally new
     set before do any walking of objects, so that only objects
     which truely still contain pointers to this generation
     are added back to the new rs. */
  
  rs = g->remember_set;
  g->remember_set = ptr_array_new(8);
  
  sz = ptr_array_length(rs);
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(ptr_array_get_index(rs, i));
    if(!REFERENCE2_P(root)) { continue; }
    root->Remember = FALSE;
    tmp = baker_gc_mutate_from(state, g, root);
    // ptr_array_set_index(g->remember_set, i, tmp);
  }
          
  
  ent = state->method_cache;
  end = ent + CPU_CACHE_SIZE;
  
  while(ent < end) {
    if(ent->klass)
      ent->klass = baker_gc_mutate_from(state, g, ent->klass);
      
    if(ent->module)
      ent->module = baker_gc_mutate_from(state, g, ent->module);
      
    if(ent->method)
      ent->method = baker_gc_mutate_from(state, g, ent->method);

    ent++;
  }
  
  /* Now the stack. */
  OBJECT *sp;
  
  sp = state->current_stack;
  while(sp <= state->current_sp) {
    tmp = *sp;
    if(REFERENCE2_P(tmp)) {
      *sp = baker_gc_mutate_from(state, g, tmp);
    }
    sp++;
  }
  
  /* Now the handle table. */
  for(i = 0; i < state->handle_tbl->total; i++) {
    if(state->handle_tbl->entries[i]) {
      state->handle_tbl->entries[i]->object = 
            baker_gc_mutate_from(state, g, state->handle_tbl->entries[i]->object);
    }
  }
  
  cpu_event_each_channel(state,
      (cpu_event_each_channel_cb) baker_gc_mutate_from, g);
  cpu_sampler_collect(state,
      (cpu_sampler_collect_cb) baker_gc_mutate_from, g);
  
  int j;
  OBJECT t2;
  for(i = 0; i < ptr_array_length(g->seen_weak_refs); i++) {
    tmp = (OBJECT)ptr_array_get_index(g->seen_weak_refs, i);
    for(j = 0; j < NUM_FIELDS(tmp); j++) {
      t2 = tuple_at(state, tmp, j);
      if(t2->gc_zone == YoungObjectZone) {
        if(baker_gc_forwarded_p(t2)) {
          tuple_put(state, tmp, j, baker_gc_forwarded_object(t2));
        } else {
          tuple_put(state, tmp, j, Qnil);
        }
      }
    }
  }
    
  baker_gc_mutate_rest(state, g);
  
  assert(heap_fully_scanned_p(g->next));
  baker_gc_swap(g);
  
  if(g->current->size != g->next->size) {
    baker_gc_enlarge_next(g, g->current->size);
  }

  if(g->used > g->current->size * 0.90) {
    DEBUG("Enlarging next!\n");
    baker_gc_enlarge_next(g, g->current->size * 1.5);
  }
  
  // printf("Saved %d contexts.\n", saved_contexts);
  ptr_array_free(rs);
  return TRUE;
}
void baker_gc_clear_marked(baker_gc g) {
  int osz;
  char *end, *cur;
  OBJECT obj;
  
  cur = (char*)g->current->address;
  end = (char*)g->current->current;
  
  while(cur < end) {
    obj = (OBJECT)cur;
    osz = SIZE_IN_BYTES(obj);
    obj->Marked = FALSE;
    cur += osz;
  }  
}

void baker_gc_find_lost_souls(STATE, baker_gc g) {
  int osz, bs;
  char *end, *cur;
  OBJECT obj, cls;
  
  cur = (char*)g->last_start;
  end = (char*)g->last_end;
  // printf("Looking for lost souls between %p and %p\n", cur, end);
  
  while(cur < end) {
    obj = (OBJECT)cur;
    osz = NUM_FIELDS(obj);
        
    if(!baker_gc_forwarded_p(obj)) {
      cls = CLASS_OBJECT(obj);
      
      //printf("%p is dead: %d, %p, %s.\n", obj, obj->RequiresCleanup);
      //  cls, cls ? _inspect(cls) : "(NULL)");
      if(obj->RequiresCleanup) {
        if(REFERENCE_P(cls) && baker_gc_forwarded_p(cls)) {
          cls = baker_gc_forwarded_object(cls);
        }
        
        state_run_cleanup(state, obj, cls);
      }
      
      if(obj->obj_type == WrapsStructType) FREE_WRAPPED_STRUCT(obj);
    }
    bs = SIZE_IN_BYTES_FIELDS(osz);
    // fast_memfill(cur, 0, SIZE_IN_WORDS_FIELDS(osz));
    // memset(cur, 0, bs);
    
    cur += bs;

  }

  // the 'next' heap has been enlarged (replaced by a new larger one).
  // free the old buffer.
  // the heap struct that holds a copy of the g->last_start pointer
  // in its 'address' slot is unreachable so can't be freed.
  if((OBJECT)g->last_start != g->next->address) {
    free(g->last_start);
  }
}

void baker_gc_collect_references(STATE, baker_gc g, OBJECT mark, ptr_array refs) {
  int sz, osz, i;
  char *end, *cur;
  OBJECT obj;
  
  sz = baker_gc_used(g);
  cur = (char*)g->current->address;
  end = cur + sz;
    
  while(cur < end) {
    obj = (OBJECT)cur;
    osz = SIZE_IN_BYTES(obj);
    
    if(!_object_stores_bytes(obj)) {
      for(i = 0; i < NUM_FIELDS(obj); i++) {
        if(NTH_FIELD(obj, i) == mark) {
          ptr_array_append(refs, (xpointer)obj);
        }
      }
    }
    
    cur += osz;
  }
}

