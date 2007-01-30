#include "shotgun.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "machine.h"
#include "baker.h"
#include "marksweep.h"

void *main_om;

static int allocated_objects = 0;

void _describe(OBJECT ptr) {
  object_memory om;
  om = (object_memory)main_om;
  printf("Address:             %p (%lu)\n", (void*)ptr, (unsigned long int)ptr);
  printf("Contained in baker?: %d/%d\n", baker_gc_contains_p(om->gc, ptr), baker_gc_contains_spill_p(om->gc, ptr));
  printf("Contained in m/s?:   %d\n", mark_sweep_contains_p(om->ms, ptr));
}

void _stats() {
  object_memory om;
  om = (object_memory)main_om;
  
  printf("Baker Info:\n");
  baker_gc_describe(om->gc);
  printf("MarkSweep Info:\n");
  mark_sweep_describe(om->ms);
}

int object_memory_actual_omsize() {
    int omsize = OMDefaultSize;
    char *s = getenv("RUBINIUS_OMSIZE");
    if (s) {
        omsize = atoi(s);
        printf("OMSize set to: %d\n", omsize);
    }
    assert(omsize > 0);         /* general sanity */
    assert((omsize & 7) == 0);  /* alignment */
    return omsize;
}

object_memory object_memory_new() {
  object_memory om;
  om = (object_memory)malloc(sizeof(struct object_memory_struct));
  memset((void*)om, 0, sizeof(struct object_memory_struct));
  om->gc = baker_gc_new(object_memory_actual_omsize());
  om->gc->tenure = (OBJECT (*)(void*,OBJECT))object_memory_tenure_object;
  om->gc->tenure_data = om;
  om->gc->om = om;
  
  om->ms = mark_sweep_new(MS_CHUNKSIZE);
  
  main_om = (void*)om;
  // Start the values up a bit higher so they don't collide
  // with the hashs of symbols right off the bat.
  om->last_object_id = 0x10000;
  // om->enlarge_new = 0;
  // om->new_size = 0;
  return om;
}

int object_memory_delete(object_memory om) {
  baker_gc_destroy(om->gc);
  return TRUE;
}

int object_memory_used(object_memory om) {
  return baker_gc_used(om->gc);
}

/*

 We need to have a write barrier that performs checks whenever a reference
 is written. This allows the new generation to collected without collecting
 the old generation too.
 
 The idea is that if +target+ is on object in the old generation, and +val+
 is an object in the new generation, we need to add +target+ to the remember
 set of new generation so that when we go to collect the new generation,
 we are sure that +val+ survives. 
 
 The flag is so that we don't put the object into the remember set
 more than once.
 */

#define REMEMBER_FLAG 0x10

/* TODO: This routine MUST be optimized because it's hit constantly. */
inline void object_memory_write_barrier(object_memory om, OBJECT target, OBJECT val) {
  if(mark_sweep_contains_p(om->ms, target) && baker_gc_contains_spill_p(om->gc, val)) {
    if(!FLAG_SET_ON_P(target, gc, REMEMBER_FLAG)) {
      g_ptr_array_add(om->gc->remember_set, (gpointer)target);
      FLAG_SET_ON(target, gc, REMEMBER_FLAG);
    }
  }
}

int object_memory_collect(object_memory om, GPtrArray *roots) {
  int i;
  // printf("%d objects since last collection.\n", allocated_objects);
  allocated_objects = 0;
  om->gc->tenure_now = om->tenure_now;
  i = baker_gc_collect(om->gc, roots);
  // object_memory_check_memory(om);
  om->gc->tenure_now = om->tenure_now = 0;
  om->collect_now = 0;
  return i;
}

void object_memory_major_collect(object_memory om, GPtrArray *roots) {
  // printf("%d objects since last collection.\n", allocated_objects);
  
  allocated_objects = 0;
  mark_sweep_collect(om->ms, roots);
  baker_gc_clear_gc_flag(om->gc, MS_MARK);
  if(om->ms->enlarged) {
    om->collect_now |= 0x2;
    om->ms->enlarged = 0;
  }
}

OBJECT object_memory_tenure_object(void *data, OBJECT obj) {
  OBJECT dest;
  object_memory om = (object_memory)data;
  
  dest = mark_sweep_allocate(om->ms, NUM_FIELDS(obj));
  /*
  HEADER(dest)->flags = HEADER(obj)->flags;
  HEADER(dest)->flags2 = HEADER(obj)->flags2;
  HEADER(dest)->flags = HEADER(obj)->flags;
  HEADER(dest)->gc = 0;
  HEADER(dest)->klass = 
  */
  memcpy((void*)dest, (void*)obj, SIZE_IN_BYTES(obj));
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
    if(om->collect_now) {
      assert(baker_gc_contains_spill_p(om->gc, obj));
    } else {
      assert(baker_gc_contains_p(om->gc, obj));
    }
  }
}

int object_memory_is_reference_p(object_memory om, OBJECT tmp) {
  return baker_gc_contains_p(om->gc, tmp) || mark_sweep_contains_p(om->ms, tmp);
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
                 num, _inspect(obj), i, fel, tmp);
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
    
    /* Stil live, must be garbage. */
    if(!baker_gc_forwarded_p(obj)) {
      printf("Found a %s that is garbage\n", _inspect(obj));
    }
        
    cur += osz;
  }
  
  // printf("Checked %d objects.\n", num);
}

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
  char *kind;
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

OBJECT object_memory_new_object(object_memory om, OBJECT cls, int fields) {
  int size, i, f;
  OBJECT obj, flags;
  struct rubinius_object *header;
  
  allocated_objects++;
  
  //fields += 4; /* PAD */
  size = (HEADER_SIZE + fields) * REFSIZE;
  if(!heap_enough_space_p(om->gc->current, size)) {
    obj = (OBJECT)baker_gc_allocate_spilled(om->gc, size);
    assert(heap_enough_space_p(om->gc->next, size));
    DEBUG("Ran out of space! spilled into %p\n", obj);
    om->collect_now |= 1;
    // baker_gc_enlarge_next(om->gc, om->gc->current->size * GC_SCALING_FACTOR);
  } else {
    obj = (OBJECT)baker_gc_allocate(om->gc, size);
  }
  object_memory_check_ptr((void*)om, obj);
  header = (struct rubinius_object*)obj;
  assert(obj);
  header->klass = cls;
  assert(cls == header->klass);
  SET_NUM_FIELDS(obj, fields);
  if(cls && REFERENCE_P(cls)) {
    flags = class_get_instance_flags(cls);
    f = FIXNUM_TO_INT(flags);
    header->flags = f;
  } else {
    header->flags = 0;
  }
  header->flags2 = 0;
  for(i = 0; i < fields; i++) {
    rbs_set_field(om, obj, i, Qnil);
  }
  
  /* The hash is unique by default, just an auto incremented ID */
  header->hash = om->last_object_id++;
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

