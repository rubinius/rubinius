#include "object_memory.h"
#include <string.h>
#include <assert.h>

void *main_om;

object_memory object_memory_new() {
  object_memory om;
  om = (object_memory)malloc(sizeof(struct object_memory_struct));
  memset((void*)om, 0, sizeof(struct object_memory_struct));
  om->gc = baker_gc_new(OMDefaultSize);
  main_om = (void*)om;
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

int object_memory_collect(object_memory om, GPtrArray *roots) {
  int i;
  i = baker_gc_collect(om->gc, roots);
  object_memory_check_memory(om);
  return i;
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

int _object_stores_bytes(OBJECT self);

void object_memory_check_memory(object_memory om) {
  int i, j, sz, osz, fel, num;
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
        if(REFERENCE_P(tmp) && !baker_gc_contains_p(om->gc,tmp)) { 
          printf("(%p-%p) %d: %s (%d of %d) contains a bad field!!\n", 
            om->gc->current->address, om->gc->current->last,
            num, _inspect(obj), i, fel);
          assert(0);
        }
      }
    }
    
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
    if(HEADER(obj)->fields == 0) {
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
  
  //fields += 4; /* PAD */
  size = (HEADER_SIZE + fields) * 4;
  if(!heap_enough_space_p(om->gc->current, size)) {
    obj = (OBJECT)baker_gc_allocate_spilled(om->gc, size);
    DEBUG("Ran out of space! spilled into %p\n", obj);
    om->collect_now = 1;
    // baker_gc_enlarge_next(om->gc, om->gc->current->size * GC_SCALING_FACTOR);
  } else {
    obj = (OBJECT)baker_gc_allocate(om->gc, size);
  }
  object_memory_check_ptr((void*)om, obj);
  header = (struct rubinius_object*)obj;
  assert(obj);
  header->klass = cls;
  assert(cls == header->klass);
  header->fields = fields;
  if(cls && REFERENCE_P(cls)) {
    flags = class_get_instance_flags(cls);
    f = FIXNUM_TO_INT(flags);
    header->flags = f;
  } else {
    header->flags = 0;
  }
  header->flags2 = 0;
  for(i = 0; i < fields; i++) {
    SET_FIELD(obj, i, Qnil);
  }
  return obj;
}

OBJECT object_memory_new_opaque(STATE, OBJECT cls, int sz) {
  int fel;
  OBJECT obj;
  fel = sz / 4;
  if(sz % 4) fel++;
  obj = object_memory_new_object(state->om, cls, fel);
  object_make_byte_storage(state, obj);
  return obj;
}

