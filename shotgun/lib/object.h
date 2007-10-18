#ifndef SEEN_RUBINIUS_OBJECT_H
#define SEEN_RUBINIUS_OBJECT_H

OBJECT object_create_metaclass(STATE, OBJECT cls, OBJECT sup);
OBJECT object_metaclass(STATE, OBJECT obj);
int object_kind_of_p(STATE, OBJECT self, OBJECT cls);
int object_has_ivars(STATE, OBJECT self);
void object_set_has_ivars(STATE, OBJECT self);
OBJECT object_get_ivar(STATE, OBJECT self, OBJECT sym);
OBJECT object_set_ivar(STATE, OBJECT self, OBJECT sym, OBJECT val);
OBJECT object_get_ivars(STATE, OBJECT self);
OBJECT object_new(STATE);
int object_copy_bytes_into(STATE, OBJECT self, OBJECT dest, int count, int offset);
unsigned int object_hash_int(STATE, OBJECT self);
int object_stores_bytes_p(STATE, OBJECT self);
void object_make_byte_storage(STATE, OBJECT self);
void object_initialize_bytes(STATE, OBJECT self);
void object_set_tainted(STATE, OBJECT self);
int object_tainted_p(STATE, OBJECT self);
void object_set_frozen(STATE, OBJECT self);
int object_frozen_p(STATE, OBJECT self);

OBJECT object_make_weak_ref(STATE, OBJECT self);
void object_cleanup_weak_refs(STATE, OBJECT self);

static inline OBJECT object_class(STATE, OBJECT self) {
  if(REFERENCE_P(self)) {
    OBJECT cls = self->klass;
    while(REFERENCE_P(cls) && (metaclass_s_metaclass_p(state, cls) || FLAGS(cls).obj_type != ClassType)) {
      cls = class_get_superclass(cls);
    }

    return cls;
  }

  return state->global->special_classes[((uint)self) & SPECIAL_CLASS_MASK];
}

#define object_byte_start(st, self) ((char*)BYTES_OF(self))
#define object_size(st, self) SIZE_OF_BODY(self)

static inline void object_copy_body(STATE, OBJECT self, OBJECT dest) {
  int s1 = object_size(state, self);
  int s2 = object_size(state, dest);
  assert(s1 <= s2);
  
  memcpy(object_byte_start(state, dest), object_byte_start(state, self), s1);
}


#define ISA(o, c) object_kind_of_p(state, o, c)

static inline uint32_t object_get_id(STATE, OBJECT self) {
  if(REFERENCE_P(self)) {
    OBJECT meta, id;
    
    meta =  object_metaclass(state, self);
    id =    object_get_ivar(state, meta, state->global->sym_object_id);
                  
    /* Lazy allocate object's ids, since most don't need them. */
    if(NIL_P(id)) {
      id = I2N(state->om->last_object_id++);
      object_set_ivar(state, meta, state->global->sym_object_id, id);
    }
    
    return (uint32_t)id;
  } else {
    return (uint32_t)(self);
  }
}

static inline int object_copy_fields_shifted(STATE, OBJECT self, OBJECT dest, int dist) {
  int count;
  int i;
  
  count = NUM_FIELDS(self);

  for(i = 0; i < count; i++) {
    SET_FIELD(dest, dist + i, NTH_FIELD(self, i));
  }
  return TRUE;
}

static inline int object_copy_fields_from(STATE, OBJECT self, OBJECT dest, int first, int count) {
  int i, j;
  int max = NUM_FIELDS(self);
  
  for(i = first, j = 0; j < count && i < max; i++, j++) {
    SET_FIELD(dest, j, NTH_FIELD(self, i));
  }
  return TRUE;  
}

static inline void object_copy_fields(STATE, OBJECT self, OBJECT dest) {
  int i, max, j;
  
  max = NUM_FIELDS(self);
  j = NUM_FIELDS(dest);
  if(max < j) max = j;
  
  for(i = 0; i < max; i++) {
    SET_FIELD(dest, i, NTH_FIELD(self, i));
  }
}
#endif
