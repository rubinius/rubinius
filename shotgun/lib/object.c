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
  OBJECT tup;
  
  tup = tuple_new2(state, 1, self);
  FLAG_SET(tup, RefsAreWeakFlag);
  
  return tup;
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

#define REMEMBER_FLAG 0x10

void object_propgate_gc_info(STATE, OBJECT self, OBJECT dest) {
  if(GC_ZONE(dest) != GC_MATURE_OBJECTS) return;
  if(FLAG_SET_ON_P(dest, gc, REMEMBER_FLAG)) return;
    
  if(GC_ZONE(self) == GC_MATURE_OBJECTS) {
    if(FLAG_SET_ON_P(self, gc, REMEMBER_FLAG)) {
      g_ptr_array_add(state->om->gc->remember_set, (gpointer)dest);
      FLAG_SET_ON(dest, gc, REMEMBER_FLAG);
    }
  } else {
    int i;
    OBJECT tmp;
    for(i = 0; i < NUM_FIELDS(dest); i++) {
      tmp = NTH_FIELD(dest, i);
      if(!REFERENCE_P(tmp)) continue;
      
      if(GC_ZONE(tmp) == GC_YOUNG_OBJECTS) {
        g_ptr_array_add(state->om->gc->remember_set, (gpointer)dest);
        FLAG_SET_ON(dest, gc, REMEMBER_FLAG);
        /* We can return because the only setting we have is now
           correct, no need to look through all the rest. */
        return;
      }
    }  
  }
}

int object_copy_fields_from(STATE, OBJECT self, OBJECT dest, int first, int count) {
  int i, j;
  
  for(i = first, j = 0; j < count; i++, j++) {
    SET_FIELD(dest, j, NTH_FIELD(self, i));
  }
  
  return TRUE;
  
/*
  char *da;
  void *start;
  int sz;
  
  
  
  da = object_byte_start(state, dest);
  sz = count * REFSIZE;
  start = (void*)((first * REFSIZE) + object_byte_start(state, self));
  
  memcpy((void*)da, (const void *)start, (size_t)sz);
  object_propgate_gc_info(state, self, dest);
  return TRUE;
*/
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
  
  if(!REFERENCE_P(self)) {
    /* Get rid of the tag part (i.e. the part that indicate nature of self */
    if(FIXNUM_P(self)) {
      int val = FIXNUM_TO_INT(self);
      /* We do this so the 2's complement will fit into 29 bits properly. */
      if(val < 0) {
        hsh = hsh >> 1;
      }
    }
    hsh = hsh >> 2;
  } else {
    if(ISA(self, state->global->string)) {
      hsh = string_hash_int(state, self);
    } else if(HEADER(self)->hash != 0) {
      hsh = HEADER(self)->hash;
    } else {
      hsh = object_get_id(state, self);
    }
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
  if(!REFERENCE_P(self) || !object_has_ivars(state, self)) {
    tbl = hash_find(state, state->global->external_ivars, self);
    if(RTEST(tbl)) {
      return hash_find(state, tbl, sym);
    }
    return Qnil;
  }
  
  tbl = object_get_instance_variables(self);

  /* No table, no ivar! */
  if(!RTEST(tbl)) return Qnil;
  
  /*
  printf("Grab ivar '%s' (%d)\n", rbs_symbol_to_cstring(state, sym), 
    object_hash_int(state, sym));
  */
  
  /* It's a tuple, use csm */
  if(ISA(tbl, state->global->tuple)) {
    return csm_find(state, tbl, sym);
  }
  
  /* It's a normal hash, no problem. */
  val = hash_find(state, tbl, sym);
  return val;
}

OBJECT object_set_ivar(STATE, OBJECT self, OBJECT sym, OBJECT val) {
  OBJECT tbl, t2;
  
  /* Implements the external ivars table for objects that don't
     have their own space for ivars. */
  if(!REFERENCE_P(self) || !object_has_ivars(state, self)) {
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
  if(NIL_P(tbl)) {
    tbl = csm_new(state);
    object_set_instance_variables(self, tbl);
    csm_add(state, tbl, sym, val);
    return val;
  }
  
  /*
  printf("Setting ivar '%s' to %p (%d)\n", rbs_symbol_to_cstring(state, sym), val,
    object_hash_int(state, sym));
  */
  if(ISA(tbl, state->global->tuple)) {
    if(TRUE_P(csm_add(state, tbl, sym, val))) {
      return val;
    }
    /* csm_add said false, meaning there is no room. We convert
       the csm into a normal hash and use it from now on. */
    tbl = csm_into_hash(state, tbl);
    object_set_instance_variables(self, tbl);
  }
  hash_set(state, tbl, sym, val);
  return val;
}

OBJECT object_get_ivars(STATE, OBJECT self) {
  OBJECT tbl;
  
  if(!REFERENCE_P(self) || !object_has_ivars(state, self)) {
    tbl = hash_find(state, state->global->external_ivars, self);
    if(!RTEST(tbl)) {
      return Qnil;
    }
    return tbl;
  }
  
  tbl = object_get_instance_variables(self);
  if(!RTEST(tbl)) {
    return Qnil;
  }
  return tbl;
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
