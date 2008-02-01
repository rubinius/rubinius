#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/machine.h"
#include "shotgun/lib/baker.h"
#include "shotgun/lib/marksweep.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/methctx.h"

void _describe(OBJECT ptr) {
  object_memory om;
  om = (object_memory)(current_machine->s->om);
  printf("Address:             %p (%lu)\n", (void*)ptr, (unsigned long int)ptr);
  printf("Contained in baker?: %d/%d\n", baker_gc_contains_p(om->gc, ptr), baker_gc_contains_spill_p(om->gc, ptr));
  printf("Contained in m/s?:   %d\n", ptr->gc_zone == MatureObjectZone);
  if(heap_contains_p(om->contexts, ptr)) {
    printf("Is a context.\n");
    if(ptr < om->context_bottom) {
      printf("  Referenced (below bottom)\n");
    } else {
      printf("  Normal on stack (not referenced)\n");
    }
  }
  printf("stack_context_p: %d / %d\n", om_stack_context_p(om, ptr), stack_context_p(ptr));
  printf("nil klass:       %d\n", ptr->klass == Qnil);
  printf("context_refd_p:  %d\n", om_context_referenced_p(om, ptr));
  printf("in_heap:         %d\n", om_in_heap(om, ptr));
  printf("methctx_fast:    %d\n", methctx_is_fast_p(current_machine->s, ptr));
  printf("blokctx_p:       %d\n", blokctx_s_block_context_p(current_machine->s, ptr));
  printf("valid_ctx_p:     %d\n", om_valid_context_p(current_machine->s, ptr));
}

void _stats() {
  object_memory om;
  om = (object_memory)(current_machine->s->om);
  
  printf("Baker Info:\n");
  baker_gc_describe(om->gc);
  printf("MarkSweep Info:\n");
  mark_sweep_describe(om->ms);
}

void _verify(OBJECT self) {
  object_memory om;
  om = (object_memory)(current_machine->s->om);
  OBJECT tmp;
  int i, rs, refs;
  gc_zone tz, vz;
  
  printf("Verifying %p...\n", (void*)self);
  if(self->StoresBytes) {
    printf("Object stores bytes.\n");
    return;
  }
  
  rs = self->Remember;
  
  tz = self->gc_zone;
  refs = 0;
  
  for(i = 0; i < NUM_FIELDS(self); i++) {
    tmp = NTH_FIELD(self, i);
    if(!REFERENCE_P(tmp)) continue;
    
    refs++;
    vz = tmp->gc_zone;
    if(tz < vz && !rs) {
      printf("ERROR: Object %p (at %i) is IG, but no RS mark!\n", (void*)tmp, i);
    }
    
  }
  printf("Done verifying %p: %d fields, %d refs\n", (void*)self, NUM_FIELDS(self), refs);  
}

int object_memory_actual_omsize() {
    int omsize = OMDefaultSize;
    char *s = getenv("RUBINIUS_OMSIZE");
    if(s) {
        omsize = atoi(s);
        /* fprintf(stderr, "OMSize set to: %d\n", omsize); */
    }
    assert(omsize > 0);         /* general sanity */
    assert((omsize & 7) == 0);  /* alignment */
    return omsize;
}

#define CONTEXT_SIZE (1024 * 1024)

object_memory object_memory_new() {
  object_memory om;
  om = (object_memory)calloc(1, sizeof(struct object_memory_struct));
  om->gc = baker_gc_new(object_memory_actual_omsize());
  om->gc->tenure = (OBJECT (*)(void*,OBJECT))object_memory_tenure_object;
  om->gc->tenure_data = om;
  om->gc->om = om;
  
  om->ms = mark_sweep_new();
  
  om->contexts = heap_new(CONTEXT_SIZE);
  om->context_bottom = (OBJECT)(om->contexts->address);
  om->context_last = (OBJECT)((uintptr_t)om->contexts->address + CONTEXT_SIZE - (CTX_SIZE * 10));
 
  /* See object_get_id for info. */
  om->last_object_id = 0;
  // om->enlarge_new = 0;
  // om->new_size = 0;
  return om;
}

int object_memory_destroy(object_memory om) {
  baker_gc_destroy(om->gc);
  mark_sweep_destroy(om->ms);
  free(om);
  return TRUE;
}

void object_memory_formalize_contexts(STATE, object_memory om) {
  OBJECT ctx;
  
  EACH_CTX(om, ctx) {
    methctx_reference(state, ctx);
  } DONE_EACH_CTX(ctx);
  
}

void object_memory_mark_contexts(STATE, object_memory om) {
  OBJECT ctx;
  
  EACH_CTX(om, ctx) {
    mark_sweep_mark_context(state, om->ms, ctx);    
  } DONE_EACH_CTX(ctx);
  
}

void object_memory_reset_contexts(STATE, object_memory om) {
  /* reset the virtual bottom */
  om->context_bottom = (OBJECT)om->contexts->address;
  
  /* reset the top */
  om->contexts->current = om->contexts->address;
}

void object_memory_clear_marks(STATE, object_memory om) {
  OBJECT ctx;
  
  EACH_CTX(om, ctx) {
    mark_sweep_clear_mark(state, ctx);
  } DONE_EACH_CTX(ctx);
}

size_t object_memory_used(object_memory om) {
  return baker_gc_used(om->gc);
}

void object_memory_setup_become(STATE, object_memory om, OBJECT from, OBJECT to) {
  om->gc->become_from = from;
  om->gc->become_to = to;
  om->ms->become_from = from;
  om->ms->become_to = to;
}

void object_memory_clear_become(STATE, object_memory om) {
  om->gc->become_from = Qnil;
  om->gc->become_to = Qnil;
  om->ms->become_from = Qnil;
  om->ms->become_to = Qnil;
}

int object_memory_collect(STATE, object_memory om, ptr_array roots) {
  int i;
  om->gc->tenure_now = om->tenure_now;
  om->last_tenured = 0;
  i = baker_gc_collect(state, om->gc, roots);
  // object_memory_check_memory(om);
  om->gc->tenure_now = om->tenure_now = 0;
  om->collect_now = 0;
  
  return i;
}

void object_memory_major_collect(STATE, object_memory om, ptr_array roots) {
  mark_sweep_collect(state, om->ms, roots);
  baker_gc_clear_marked(om->gc);
  object_memory_clear_marks(state, om);
}

OBJECT object_memory_tenure_object(void *data, OBJECT obj) {
  OBJECT dest;
  object_memory om = (object_memory)data;
  mark_sweep_gc ms = om->ms;
  
  om->last_tenured++;
  
  dest = mark_sweep_allocate(ms, NUM_FIELDS(obj));
  
  // printf("Tenuring %p to %p (%d)\n", (void*)obj, (void*)dest, NUM_FIELDS(obj));
  
  if(ms->enlarged) { 
    om->collect_now |= OMCollectMature;
  }
  
  fast_memcpy((void*)dest, (void*)obj, SIZE_IN_WORDS_FIELDS(NUM_FIELDS(obj)));
  dest->gc_zone = MatureObjectZone;
  //printf("Allocated %d fields to %p\n", NUM_FIELDS(obj), obj);
  // printf(" :: %p => %p (%d / %d )\n", obj, dest, NUM_FIELDS(obj), SIZE_IN_BYTES(obj));
  return dest;
}

void object_memory_print_stats(object_memory om) {
  printf("Memory: %zd used, %zd total.\n", object_memory_used(om), om->gc->current->size);
}

void object_memory_check_ptr(void *ptr, OBJECT obj) {
  object_memory om = (object_memory)ptr;
  if(REFERENCE_P(obj)) {
    assert(baker_gc_contains_spill_p(om->gc, obj) ||
           mark_sweep_contains_p(om->ms, obj) ||
           heap_contains_p(om->contexts, obj));
    assert(obj->klass != Qnil);
  } else if(SYMBOL_P(obj)) {
    assert((uintptr_t)obj < 10000000);
  }
}

void object_memory_update_rs(object_memory om, OBJECT target, OBJECT val) {
  if(!target->Remember) {
    // printf("[Tracking %p in baker RS]\n", (void*)target);
    ptr_array_append(om->gc->remember_set, (xpointer)target);
    target->Remember = TRUE;
  }
}

int object_memory_is_reference_p(object_memory om, OBJECT tmp) {
  return baker_gc_contains_p(om->gc, tmp) || mark_sweep_contains_p(om->ms, tmp);
}

OBJECT object_memory_collect_references(STATE, object_memory om, OBJECT mark) {
  ptr_array refs;
  int i;
  OBJECT tup;
  
  refs = ptr_array_new(8);
  
  baker_gc_collect_references(state, om->gc, mark, refs);
  mark_sweep_collect_references(state, om->ms, mark, refs);
  
  if(ptr_array_length(refs) == 0) return Qnil;
  
  tup = tuple_new(state, ptr_array_length(refs));
  for(i = 0; i < ptr_array_length(refs); i++) {
    tuple_put(state, tup, i, (OBJECT)ptr_array_get_index(refs, i));
  }
    
  return tup;
  
}

int _object_stores_bytes(OBJECT self);

/*
 * consistency check the object_memory: all objects either store bytes
 * (in which case we don't care about their contents) or they store
 * fields that are valid objects.
 *
 * object_memory_check_memory only works correctly if om->collect_now
 * is false; as the function object_memory_is_reference_p assumes
 * there haven't been any spills yet.  [Bug or feature?]
 */

void object_memory_check_memory(object_memory om) {
  int i, sz, osz, fel, num;
  char *start, *end, *cur;
  OBJECT obj, tmp;
  
  sz = object_memory_used(om);
  start = (char*)om->gc->current->address;
  end = start + sz;
  cur = start;
  
  num = 0;
  
  while(cur < end) {
    num++;
    obj = (OBJECT)cur;
    osz = SIZE_IN_BYTES(obj);
    
    fel = NUM_FIELDS(obj);
    if(!_object_stores_bytes(obj)) {
      for(i = 0; i < fel; i++) {
        tmp = NTH_FIELD_DIRECT(obj, i);
        if(REFERENCE_P(tmp) && !object_memory_is_reference_p(om,tmp)) { 
          printf("(%p-%p) %d: %s (%d of %d) contains a bad field (%p)!!\n", 
            (void*)om->gc->current->address, (void*)om->gc->current->last,
                 num, _inspect(obj), i, fel, (void*)tmp);
          /* separate printf as this one might segfault: */
          printf("bad field is: %s\n", _inspect(tmp));
          assert(0);
        }
      }
    }
    
    cur += osz;
  }
  
  // printf("Checked %d objects.\n", num);
}

/*
void object_memory_detect_cleanup(object_memory om) {
  int sz, osz, num;
  char *start, *end, *cur;
  OBJECT obj;
  
  sz = object_memory_used(om);
  start = (char*)om->gc->current->address;
  end = start + sz;
  cur = start;
  
  num = 0;
  
  while(cur < end) {
    num++;
    obj = (OBJECT)cur;
    osz = SIZE_IN_BYTES(cur);
    
    if(!baker_gc_forwarded_p(obj)) {
      printf("Found a %s that is garbage\n", _inspect(obj));
    }
        
    cur += osz;
  }  
}
*/

void state_each_object(STATE, OBJECT kls, void (*cb)(STATE, OBJECT)) {
  int sz, osz, num;
  char *start, *end, *cur;
  OBJECT obj;
  object_memory om;
  
  om = state->om;
  
  sz = object_memory_used(om);
  start = (char*)om->gc->current->address;
  end = start + sz;
  cur = start;
  
  num = 0;
  
  while(cur < end) {
    num++;
    obj = (OBJECT)cur;
    osz = SIZE_IN_BYTES(obj);
    
    (*cb)(state, obj);
    
    cur += osz;
  }
  
  // printf("Checked %d objects.\n", num);
}

void object_memory_emit_details(STATE, object_memory om, FILE *stream) {
  int sz, osz;
  char *start, *end, *cur;
  const char *kind;
  OBJECT obj, kls;
  
  sz = object_memory_used(om);
  start = (char*)om->gc->current->address;
  end =   start + sz;
  
  cur = start;
  while(cur < end) {
    obj = (OBJECT)cur;
    osz = SIZE_IN_BYTES(obj);
    if(NUM_FIELDS(obj) == 0) {
      fprintf(stream, "%p %d free\n", obj, (int)(end - cur));
    }
    kls = obj->klass;
    if(kls == state->global->cmethod) {
      kind = "cmethod";
    } else if(kls == state->global->bytearray) {
      kind = "bytearray";
    } else if(kls == state->global->string) {
      kind = "string";
    } else if(kls == state->global->tuple) {
      kind = "tuple";
    } else if(kls == state->global->methctx || kls == state->global->blokctx) {
      kind = "context";
    } else if(kls == state->global->class || kls == state->global->metaclass) {
      kind = "class";
    } else {
      kind = "unknown";
    }
    
    fprintf(stream, "%p %d %s\n", obj, osz, kind);
    cur += osz;
  }
  fclose(stream);
}

OBJECT object_memory_new_object_mature(object_memory om, OBJECT cls, unsigned int fields) {
  int i;
  OBJECT obj;
  mark_sweep_gc ms;
  
  ms = om->ms;
  
  obj = mark_sweep_allocate(ms, fields);
  if(ms->enlarged) { 
    om->collect_now |= OMCollectMature;
  }
  
  CLEAR_FLAGS(obj);
  
  obj->gc_zone = MatureObjectZone;
  
  rbs_set_class(om, obj, cls);
  SET_NUM_FIELDS(obj, fields);
  if(cls && REFERENCE_P(cls)) {
    _om_apply_class_flags(obj, cls);
  }
  for(i = 0; i < fields; i++) {
    rbs_set_field(om, obj, i, Qnil);
  }
  
  
  return obj;
}

OBJECT object_memory_new_object_normal(object_memory om, OBJECT cls, unsigned int fields) {
  int size, i;
  OBJECT obj;
    
  //fields += 4; /* PAD */
  size = SIZE_IN_BYTES_FIELDS(fields);
  if(!heap_enough_space_p(om->gc->current, size)) {
    obj = (OBJECT)baker_gc_allocate_spilled(om->gc, size);
    xassert(heap_enough_space_p(om->gc->next, size));
    // DEBUG("Ran out of space! spilled into %p\n", obj);
    om->collect_now |= OMCollectYoung;
    // baker_gc_enlarge_next(om->gc, om->gc->current->size * GC_SCALING_FACTOR);
  } else {
    obj = (OBJECT)baker_gc_allocate(om->gc, size);
  }
  
  CLEAR_FLAGS(obj);
  obj->gc_zone = YoungObjectZone;
  
  rbs_set_class(om, obj, cls);
  SET_NUM_FIELDS(obj, fields);
  if(cls && REFERENCE_P(cls)) {
    _om_apply_class_flags(obj, cls);
  }
  
  for(i = 0; i < fields; i++) {
    rbs_set_field(om, obj, i, Qnil);
  }
  
  return obj;
}

OBJECT object_memory_new_opaque(STATE, OBJECT cls, unsigned int sz) {
  unsigned int fel;
  OBJECT obj;
  fel = sz / SIZE_OF_OBJECT;
  if(sz % SIZE_OF_OBJECT) fel++;
  obj = object_memory_new_object(state->om, cls, fel);
  object_make_byte_storage(state, obj);
  return obj;
}

