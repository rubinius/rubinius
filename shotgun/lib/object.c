#include "shotgun.h"
#include "metaclass.h"
#include <string.h>
#include <stdlib.h>
#include "hash.h"
#include "string.h"
#include "flags.h"
#include "tuple.h"

OBJECT object_new(STATE) {
  return object_allocate(state);
}

OBJECT object_create_metaclass(STATE, OBJECT cls, OBJECT sup) {
  OBJECT meta;
  if(!sup) {
    sup = HEADER(cls)->klass;
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
  while(REFERENCE_P(cls) && metaclass_s_metaclass_p(state, cls)) {
    cls = class_get_superclass(cls);
  }
  
  return cls;
}

/*
void rbs_show_classes(STATE, OBJECT self) {
  OBJECT cls = HEADER(self)->klass;
  printf("Class from %p\n", (void *)self);
  while(REFERENCE_P(cls) && metaclass_s_metaclass_p(state, cls)) {
    printf(" => %d %p %s\n",
        HEADER(cls)->klass == state->global->metaclass,
        (void *)cls,
        rbs_symbol_to_cstring(state,
          class_get_name(object_logical_class(state, cls))));
    cls = class_get_superclass(cls);
  }
  printf(" found => %p\n", (void *)cls);
}
*/

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

  if(self == Qundef) {
    return state->global->undef_class;
  }
  
  printf("Unable to figure out logical class.\n");
  abort();
  return Qnil;
}

OBJECT object_make_weak_ref(STATE, OBJECT self) {
  OBJECT meta, tup, lst;
  
  tup = tuple_new2(state, 1, self);
  FLAG_SET(tup, RefsAreWeakFlag);
  FLAG_SET(self, HasWeakRefsFlag);

  meta = object_metaclass(state, self);
  
  lst = class_get_instance_fields(meta);
  if(NIL_P(lst)) {
    lst = tuple_new2(state, 1, tup);
  } else {
    lst = tuple_enlarge(state, tup, 1);
    tuple_put(state, lst, NUM_FIELDS(lst) - 1, tup);
  }
  
  class_set_instance_fields(meta, lst);
  
  return tup;
}

void object_cleanup_weak_refs(STATE, OBJECT self) {
  OBJECT meta, lst, tup;
  int i;
  
  meta = object_metaclass(state, self);
  lst = class_get_instance_fields(meta);
  
  for(i = 0; i < NUM_FIELDS(lst); i++) {
    tup = tuple_at(state, lst, i);
    tuple_put(state, 0, tup, Qnil);
  }
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
  sz = count * REFSIZE;
  start = (void*)((first * REFSIZE) + object_byte_start(state, self));
  
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
  
  memcpy((void*)start, object_byte_start(state, self), count);
  return TRUE;
}

unsigned int object_hash_int(STATE, OBJECT self) {
  unsigned int hsh;
  hsh = (unsigned int)self;
  
  if(SYMBOL_P(self) || FIXNUM_P (self)) {
      /* Get rid of the tag part (i.e. the part that indicate nature of self */
      hsh = hsh >> 2;
  } else if(REFERENCE_P(self) && HEADER(self)->klass == state->global->string) {
    hsh = string_hash_int(state, self);
  } else {
    hsh = HEADER(self)->hash;
  }
  
  return hsh;
}

int object_has_ivars(STATE, OBJECT self) {
  if(FLAG_SET_P(self, CanStoreIvarsFlag)) {
    /* TODO: here we could check that the 1st field
       is currently storing a Hash for the ivars and also
       use that to determine if this object has ivars. */
    return TRUE;
  }
  
  return FALSE;
}

void object_set_has_ivars(STATE, OBJECT self) {
  FLAG_SET(self, CanStoreIvarsFlag);
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

void object_set_tainted(STATE, OBJECT self) {
  if(!REFERENCE_P(self)) return;
	FLAG2_SET(self, IsTaintedFlag);
}

int object_tainted_p(STATE, OBJECT self) {
  if(REFERENCE_P(self) && FLAG2_SET_P(self, IsTaintedFlag)) {
    return TRUE;
  }
  return FALSE;
}

void object_set_frozen(STATE, OBJECT self) {
  if(!REFERENCE_P(self)) return;
  FLAG2_SET(self, IsFrozenFlag);
}

int object_frozen_p(STATE, OBJECT self) {
  if(REFERENCE_P(self) && FLAG2_SET_P(self, IsFrozenFlag)) {
    return TRUE;
  }
  return FALSE;
}
