#include "object_memory.h"
#include <string.h>
#include <assert.h>

object_memory object_memory_new() {
  object_memory om;
  om = (object_memory)malloc(sizeof(struct object_memory_struct));
  memset((void*)om, 0, sizeof(struct object_memory_struct));
  om->gc = baker_gc_new(OMDefaultSize);
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
  return baker_gc_collect(om->gc, roots);
}

void object_memory_print_stats(object_memory om) {
  printf("Memory: %d used, %d total.\n", object_memory_used(om), om->gc->current->size);
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
  
  size = (HEADER_SIZE + fields) * 4;
  obj = (OBJECT)baker_gc_allocate(om->gc, size);
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

