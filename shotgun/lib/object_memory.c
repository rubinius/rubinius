#include "shotgun.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "machine.h"
#include "baker.h"
#include "marksweep.h"
#include "tuple.h"
#include "flags.h"
#include "methctx.h"

void *main_om;

static int allocated_objects = 0;

extern machine current_machine;

void _describe(OBJECT ptr) {
  object_memory om;
  om = (object_memory)main_om;
  printf("Address:             %p (%lu)\n", (void*)ptr, (unsigned long int)ptr);
  printf("Contained in baker?: %d/%d\n", baker_gc_contains_p(om->gc, ptr), baker_gc_contains_spill_p(om->gc, ptr));
  printf("Contained in m/s?:   %d\n", GC_ZONE(ptr) == GC_MATURE_OBJECTS);
  if(heap_contains_p(om->contexts, ptr)) {
    printf("Is a context.\n");
    if(ptr < om->context_bottom) {
      printf("  Referenced (below bottom)\n");
    } else {
      printf("  Normal on stack (not referenced)\n");
    }
  }
  printf("stack_context_p: %d\n", om_stack_context_p(om, ptr));
  printf("nil klass:       %d\n", HEADER(ptr)->klass == Qnil);
  printf("context_refd_p:  %d\n", om_context_referenced_p(om, ptr));
  printf("in_heap:         %d\n", om_in_heap(om, ptr));
  printf("methctx_fast:    %d\n", methctx_is_fast_p(current_machine->s, ptr));
  printf("blokctx_p:       %d\n", blokctx_s_block_context_p(current_machine->s, ptr));
  printf("valid_ctx_p:     %d\n", om_valid_context_p(current_machine->s, ptr));
}

void _stats() {
  object_memory om;
  om = (object_memory)main_om;
  
  printf("Baker Info:\n");
  baker_gc_describe(om->gc);
  printf("MarkSweep Info:\n");
  mark_sweep_describe(om->ms);
}

void _verify(OBJECT self) {
  object_memory om;
  om = (object_memory)main_om;
  OBJECT tmp;
  int i, rs, tz, vz, refs;
  
  printf("Verifying %p...\n", (void*)self);
  if(FLAG_SET_P(self, StoresBytesFlag)) {
    printf("Object stores bytes.\n");
    return;
  }
  
  rs = FLAG_SET_ON_P(self, gc, 0x10);
  
  tz = GC_ZONE(self);
  refs = 0;
  
  for(i = 0; i < NUM_FIELDS(self); i++) {
    tmp = NTH_FIELD(self, i);
    if(!REFERENCE_P(tmp)) continue;
    
    refs++;
    vz = GC_ZONE(tmp);
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
  om->context_last = (OBJECT)om->contexts->address + CONTEXT_SIZE - (CTX_SIZE * 3);
  
  main_om = (void*)om;
  // Start the values up a bit higher so they don't collide
  // with the hashs of symbols right off the bat.
  om->last_object_id = 0x10000;
  // om->enlarge_new = 0;
  // om->new_size = 0;
  return om;
}

void object_memory_formalize_contexts(STATE, object_memory om) {
  OBJECT ctx;
  
  EACH_REFD_CTX(om, ctx) {
    methctx_reference(state, ctx);
  } DONE_EACH_REFD_CTX(ctx);
  
  /*
  char *addr;
  addr = (char*)(om->contexts->address);
  
  while(addr < (char*)om->context_bottom) {
    ctx = (OBJECT)addr;
    methctx_reference(state, ctx);
    addr += CTX_SIZE;
  }
  */
}

void object_memory_shift_contexts(STATE, object_memory om) {
  OBJECT ctx, new_ctx;
  int inc = 0;
  // char *addr, *cur;
  struct fast_context *fc;
  
  // addr = (char*)(om->context_bottom);
  // cur = (char*)(om->contexts->address);
  
  /* If the context_bottom is the true bottom, we haven't promoted
     anything and everything can stay where it is. */
  if(om_no_referenced_ctxs_p(om)) {
    // printf("Nothing to shift!\n");
    om->context_top = Qnil;
    
    /* Fixup the refs that are in the context stack */
    EACH_STACK_CTX(om, ctx) {
      baker_gc_mutate_context(state, om->gc, ctx, FALSE, inc == 0);
      inc++;
    } DONE_EACH_STACK_CTX(ctx); 
    /*
    while(addr < (char*)om->contexts->current) {
      ctx = (OBJECT)addr;
      xassert(HEADER(ctx)->klass == Qnil);
      addr += CTX_SIZE;
    } 
    */   
  } else {
    
    new_ctx = (OBJECT)(om->contexts->address);
    
    EACH_STACK_CTX(om, ctx) {
      /* The top context is a little special. Either it's sender
         is nil or in the heap. Let mutate context know this is the case */
      if(inc == 0) {
        baker_gc_mutate_context(state, om->gc, ctx, TRUE, TRUE);
        memcpy((void*)new_ctx, (void*)ctx, CTX_SIZE);
        HEADER(ctx)->klass = new_ctx;
      } else {
        memcpy((void*)new_ctx, (void*)ctx, CTX_SIZE);
        HEADER(ctx)->klass = new_ctx;
        baker_gc_mutate_context(state, om->gc, new_ctx, TRUE, FALSE);        
      }
      new_ctx += CTX_SIZE;
      inc++;
    } DONE_EACH_STACK_CTX(ctx);
    
    om->contexts->current = (address)new_ctx;
    om->context_top = new_ctx - CTX_SIZE;
    
    /*
    
    while(addr < (char*)om->contexts->current) {
      ctx = (OBJECT)cur;
        
      memcpy(cur, addr, CTX_SIZE);
      // printf("Shifted context %p to %p\n", addr, cur);
    
      HEADER(addr)->klass = ctx;
    
      xassert(HEADER(ctx)->klass == Qnil);
      baker_gc_mutate_context(state, om->gc, ctx, TRUE);
    
      cur += CTX_SIZE;
      addr += CTX_SIZE;
    }
    
     Reset current to be the moved top 
    om->contexts->current = (address)(cur);
    */
  }
  
  om->context_bottom = (OBJECT)(om->contexts->address);
  
}

void object_memory_mark_contexts(STATE, object_memory om) {
  OBJECT ctx;
  
  EACH_CTX(om, ctx) {
    mark_sweep_mark_context(state, om->ms, ctx);    
  } DONE_EACH_CTX(ctx);
  
  /*
  
  OBJECT ctx, lst;
  char *addr, *cur;
  struct fast_context *fc;
  
  addr = (char*)(om->contexts->address);
  
  while(addr < (char*)om->contexts->current) {
    ctx = (OBJECT)addr;
    addr += CTX_SIZE;
  }
  */ 
}

void object_memory_clear_marks(STATE, object_memory om) {
  OBJECT ctx, lst;
  char *addr, *cur;
  struct fast_context *fc;
  
  addr = (char*)(om->contexts->address);
  
  /* Fixup the refs that are in the context stack */
  while(addr < (char*)om->contexts->current) {
    ctx = (OBJECT)addr;
    mark_sweep_clear_mark(state, ctx);
    addr += CTX_SIZE;
  } 
}


int object_memory_delete(object_memory om) {
  baker_gc_destroy(om->gc);
  return TRUE;
}

int object_memory_used(object_memory om) {
  return baker_gc_used(om->gc);
}

void object_memory_setup_become(STATE, object_memory om, OBJECT from, OBJECT to) {
  om->gc->become_from = from;
  om->gc->become_to = to;
  om->ms->become_from = from;
  om->ms->become_to = to;
}

void object_memory_clear_become(STATE, object_memory om) {
  om->gc->become_from = 0;
  om->gc->become_to = 0;
  om->ms->become_from = 0;
  om->ms->become_to = 0;
}

int object_memory_collect(STATE, object_memory om, GPtrArray *roots) {
  int i;
  // printf("%d objects since last collection.\n", allocated_objects);
  allocated_objects = 0;
  om->gc->tenure_now = om->tenure_now;
  om->last_tenured = 0;
  i = baker_gc_collect(state, om->gc, roots);
  // object_memory_check_memory(om);
  om->gc->tenure_now = om->tenure_now = 0;
  om->collect_now = 0;
  
  return i;
}

void object_memory_major_collect(STATE, object_memory om, GPtrArray *roots) {
  // printf("%d objects since last collection.\n", allocated_objects);
  
  allocated_objects = 0;
  mark_sweep_collect(state, om->ms, roots);
  baker_gc_clear_gc_flag(om->gc, MS_MARK);
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
  
  fast_memcpy((void*)dest, (void*)obj, NUM_FIELDS(obj) + HEADER_SIZE);
  GC_ZONE_SET(dest, GC_MATURE_OBJECTS);
  //printf("Allocated %d fields to %p\n", NUM_FIELDS(obj), obj);
  // printf(" :: %p => %p (%d / %d )\n", obj, dest, NUM_FIELDS(obj), SIZE_IN_BYTES(obj));
  return dest;
}

void object_memory_print_stats(object_memory om) {
  printf("Memory: %d used, %d total.\n", object_memory_used(om), om->gc->current->size);
}

void object_memory_check_ptr(void *ptr, OBJECT obj) {
  object_memory om = (object_memory)ptr;
  if(REFERENCE_P(obj)) {
    assert(baker_gc_contains_spill_p(om->gc, obj) ||
           mark_sweep_contains_p(om->ms, obj) ||
           heap_contains_p(om->contexts, obj));
    assert(HEADER(obj)->klass != Qnil);
  } else if(SYMBOL_P(obj)) {
    assert(obj < 10000000);
  }
}

void object_memory_update_rs(object_memory om, OBJECT target, OBJECT val) {
  if(!FLAG_SET_ON_P(target, gc, REMEMBER_FLAG)) {
    // printf("[Tracking %p in baker RS]\n", (void*)target);
    g_ptr_array_add(om->gc->remember_set, (gpointer)target);
    FLAG_SET_ON(target, gc, REMEMBER_FLAG);
  }
}

int object_memory_is_reference_p(object_memory om, OBJECT tmp) {
  return baker_gc_contains_p(om->gc, tmp) || mark_sweep_contains_p(om->ms, tmp);
}

OBJECT object_memory_collect_references(STATE, object_memory om, OBJECT mark) {
  GPtrArray *refs;
  int i;
  OBJECT tup;
  
  /* Broken. Causes segfaults. Not sure why yet. */
  return Qnil;
  
  
  refs = g_ptr_array_new();
  
  baker_gc_collect_references(state, om->gc, mark, refs);
  mark_sweep_collect_references(state, om->ms, mark, refs);
  
  
  if(refs->len == 0) return Qnil;
  
  tup = tuple_new(state, refs->len);
  for(i = 0; i < refs->len; i++) {
    tuple_put(state, tup, i, (OBJECT)g_ptr_array_index(refs, i));
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
    osz = SIZE_IN_BYTES(cur);
    
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
    osz = SIZE_IN_BYTES(cur);
    
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
    osz = SIZE_IN_BYTES(cur);
    if(NUM_FIELDS(obj) == 0) {
      fprintf(stream, "%d %d free\n", (int)obj, end - cur);
    }
    kls = HEADER(obj)->klass;
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
    
    fprintf(stream, "%d %d %s\n", (int)obj, osz, kind);
    cur += osz;
  }
  fclose(stream);
}

OBJECT object_memory_new_object_mature(object_memory om, OBJECT cls, int fields) {
  int i, f;
  OBJECT obj, flags;
  struct rubinius_object *header;
  mark_sweep_gc ms;
  
  ms = om->ms;
  
  obj = mark_sweep_allocate(ms, fields);
  if(ms->enlarged) { 
    om->collect_now |= OMCollectMature;
  }
  
  header = (struct rubinius_object*)obj;  
  header->flags2 = 0;
  header->gc = 0;
  
  GC_ZONE_SET(obj, GC_MATURE_OBJECTS);
  
  rbs_set_class(om, obj, cls);
  SET_NUM_FIELDS(obj, fields);
  if(cls && REFERENCE_P(cls)) {
    flags = class_get_instance_flags(cls);
    f = FIXNUM_TO_INT(flags);
    header->flags = f;
  } else {
    header->flags = 0;
  }
  for(i = 0; i < fields; i++) {
    rbs_set_field(om, obj, i, Qnil);
  }
  
  
  return obj;
}

OBJECT object_memory_new_object_normal(object_memory om, OBJECT cls, int fields) {
  int size, i, f;
  OBJECT obj, flags;
  struct rubinius_object *header;
    
  //fields += 4; /* PAD */
  size = (HEADER_SIZE + fields) * REFSIZE;
  if(!heap_enough_space_p(om->gc->current, size)) {
    obj = (OBJECT)baker_gc_allocate_spilled(om->gc, size);
    xassert(heap_enough_space_p(om->gc->next, size));
    // DEBUG("Ran out of space! spilled into %p\n", obj);
    om->collect_now |= OMCollectYoung;
    // baker_gc_enlarge_next(om->gc, om->gc->current->size * GC_SCALING_FACTOR);
  } else {
    obj = (OBJECT)baker_gc_allocate(om->gc, size);
  }
  
  header = (struct rubinius_object*)obj;
  header->flags2 = 0;
  header->gc = 0;
  
  GC_ZONE_SET(obj, GC_YOUNG_OBJECTS);
  
  rbs_set_class(om, obj, cls);
  SET_NUM_FIELDS(obj, fields);
  if(cls && REFERENCE_P(cls)) {
    flags = class_get_instance_flags(cls);
    f = FIXNUM_TO_INT(flags);
    header->flags = f;
  } else {
    header->flags = 0;
  }
  
  for(i = 0; i < fields; i++) {
    rbs_set_field(om, obj, i, Qnil);
  }
  
  return obj;
}

OBJECT object_memory_new_opaque(STATE, OBJECT cls, int sz) {
  int fel;
  OBJECT obj;
  fel = sz / REFSIZE;
  if(sz % REFSIZE) fel++;
  obj = object_memory_new_object(state->om, cls, fel);
  object_make_byte_storage(state, obj);
  return obj;
}

