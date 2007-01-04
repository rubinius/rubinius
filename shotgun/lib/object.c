#include "shotgun.h"
#include "metaclass.h"
#include <string.h>
#include <stdlib.h>
#include "hash.h"
#include "string.h"

OBJECT object_new(STATE) {
  return object_allocate(state);
  // return NEW_OBJECT(BASIC_CLASS(object), 1);
}

OBJECT object_create_metaclass(STATE, OBJECT cls, OBJECT sup) {
  OBJECT meta;
  if(!sup) {
    sup = HEADER(cls)->klass;
    // sup = object_metaclass(state, class_get_superclass(cls));
  }
  
  meta = metaclass_s_attach(state, cls);
  class_set_superclass(meta, sup);
  
  return meta;
}

OBJECT object_metaclass(STATE, OBJECT obj) {
  if(metaclass_s_metaclass_p(state, HEADER(obj)->klass)) {
    return HEADER(obj)->klass;
  }
  return object_create_metaclass(state, obj, 0);
}

OBJECT object_class(STATE, OBJECT self) {
  OBJECT cls = HEADER(self)->klass;
  // printf("Looking for class of %x (%x).\n", self, cls);
  while(REFERENCE_P(cls) && metaclass_s_metaclass_p(state, cls)) {
    cls = class_get_superclass(cls);
    //printf("Trying %x (%d).\n", cls);
  }
  
  return cls;
}

OBJECT object_logical_class(STATE, OBJECT self) {
  if(REFERENCE_P(self)) {
    return object_class(state, self);
  }
  
  if(FIXNUM_P(self)) {
    return state->global->fixnum_class;
  }
  
  if(SYMBOL_P(self)) {
    return state->global->symbol;
  }
  
  if(self == Qtrue) {
    return state->global->true_class;
  }
  
  if(self == Qfalse) {
    return state->global->false_class;
  }
  
  if(self == Qnil) {
    return state->global->nil_class;
  }
  
  printf("Unable to figure out logical class.\n");
  abort();
  return Qnil;
}

int object_kind_of_p(STATE, OBJECT self, OBJECT cls) {
  OBJECT found;
  
  found = object_logical_class(state, self);
  if(found == cls) {
    return TRUE;
  }
  
  /* Normal class inheritence checking. */
  
  while(RTEST(found)) {
    found = class_get_superclass(found);
    if(found == cls) { return TRUE; }
  }
  
  return FALSE;
}

char *object_byte_start(STATE, OBJECT self) {
  return BYTES_OF(self);
}

int object_copy_fields_from(STATE, OBJECT self, OBJECT dest, int first, int count) {
  char *da;
  void *start;
  int sz;
  
  da = object_byte_start(state, dest);
  sz = count * 4;
  start = (void*)((first * 4) + object_byte_start(state, self));
  
  memcpy((void*)da, (const void *)start, (size_t)sz);
  return TRUE;
}

int object_copy_fields_shifted(STATE, OBJECT self, OBJECT dest, int dist) {
  int count;
  int i;

  count = NUM_FIELDS(self);
  
  for(i = 0; i < count; i++) {
    SET_FIELD(dest, dist + i, NTH_FIELD(self, i));
  }
  
  return TRUE;
}


int object_copy_bytes_into(STATE, OBJECT self, OBJECT dest, int count, int offset) {
  char *str, *start;
  
  str = (char*)object_byte_start(state, dest);
  start = str + offset;
  
  assert(count >= 0);
  memcpy((void*)start, object_byte_start(state, self), count);
  return TRUE;
}

unsigned int object_hash_int(STATE, OBJECT self) {
  unsigned int hsh;
  hsh = (unsigned int)self;
  
  if(SYMBOL_P(self)) {
    hsh = hsh >> 2;
  } else if(REFERENCE_P(self) && HEADER(self)->klass == state->global->string) {
    hsh = string_hash_int(state, self);
  }
  
  return hsh;
}

#define CanStoreIvars 0x02

int object_has_ivars(STATE, OBJECT self) {
  if(FLAG_SET_P(self, CanStoreIvars)) {
    /* TODO: here we could check that the 1st field
       is currently storing a Hash for the ivars and also
       use that to determine if this object has ivars. */
    return TRUE;
  }
  
  return FALSE;
}

void object_set_has_ivars(STATE, OBJECT self) {
  FLAG_SET(self, CanStoreIvars);
}

OBJECT object_get_ivar(STATE, OBJECT self, OBJECT sym) {
  OBJECT tbl, val;

  /* Implements the external ivars table for objects that don't
     have their own space for ivars. */
  if(!object_has_ivars(state, self)) {
    tbl = hash_find(state, state->global->external_ivars, self);
    if(RTEST(tbl)) {
      return hash_find(state, tbl, sym);
    }
    return Qnil;
  }
  
  tbl = object_get_instance_variables(self);
  
  /* Lazy creation of hash to store instance variables. */
  if(!RTEST(tbl)) {
    tbl = hash_new(state);
    object_set_instance_variables(self, tbl);
  }
  
  /*
  printf("Grab ivar '%s' (%d)\n", rbs_symbol_to_cstring(state, sym), 
    object_hash_int(state, sym));
  */
  
  val = hash_find(state, tbl, sym);
  return val;
}

OBJECT object_set_ivar(STATE, OBJECT self, OBJECT sym, OBJECT val) {
  OBJECT tbl, t2;
  
  /* Implements the external ivars table for objects that don't
     have their own space for ivars. */
  if(!object_has_ivars(state, self)) {
    tbl = hash_find(state, state->global->external_ivars, self);
    
    /* Lazy creation of the hash table for the object. */
    /* HACK: This breaks any garbage collection of these objects
         because the GC will see the objects has keys here even
         if they are not referenced elsewhere... */
    
    if(!RTEST(tbl)) {
      t2 = hash_new(state);
      hash_set(state, state->global->external_ivars, self, t2);
      tbl = t2;
    }
    hash_set(state, tbl, sym, val);
    return val;
  }
  
  tbl = object_get_instance_variables(self);
  
  /* Lazy creation of hash to store instance variables. */
  if(!RTEST(tbl)) {
    tbl = hash_new(state);
    object_set_instance_variables(self, tbl);
  }
  
  /*
  printf("Setting ivar '%s' to %p (%d)\n", rbs_symbol_to_cstring(state, sym), val,
    object_hash_int(state, sym));
  */
  
  hash_set(state, tbl, sym, val);
  return val;
}

#define StoresBytesFlag 0x04
int object_stores_bytes_p(STATE, OBJECT self) {
  if(!REFERENCE_P(self)) return FALSE;
  if(FLAG_SET_P(self, StoresBytesFlag)) return TRUE;
  return FALSE;
}

int _object_stores_bytes(OBJECT self) {
  if(FLAG_SET_P(self, StoresBytesFlag)) return TRUE;
  return FALSE;  
}

void object_make_byte_storage(STATE, OBJECT self) {
  FLAG_SET(self, StoresBytesFlag);
}

void object_initialize_bytes(STATE, OBJECT self) {
  int sz;
  sz = SIZE_OF_BODY(self);
  memset(object_byte_start(state, self), 0, sz);
}
