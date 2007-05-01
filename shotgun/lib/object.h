#ifndef SEEN_RUBINIUS_OBJECT_H
#define SEEN_RUBINIUS_OBJECT_H

OBJECT object_create_metaclass(STATE, OBJECT cls, OBJECT sup);
OBJECT object_metaclass(STATE, OBJECT obj);
OBJECT object_class(STATE, OBJECT self);
OBJECT object_logical_class(STATE, OBJECT self);
int object_kind_of_p(STATE, OBJECT self, OBJECT cls);
int object_has_ivars(STATE, OBJECT self);
void object_set_has_ivars(STATE, OBJECT self);
OBJECT object_get_ivar(STATE, OBJECT self, OBJECT sym);
OBJECT object_set_ivar(STATE, OBJECT self, OBJECT sym, OBJECT val);
OBJECT object_new(STATE);
char *object_byte_start(STATE, OBJECT self);
int object_copy_fields_from(STATE, OBJECT self, OBJECT dest, int first, int count);
int object_copy_fields_shifted(STATE, OBJECT self, OBJECT dest, int dist);
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

#define ISA(o, c) object_kind_of_p(state, o, c)

#define WEAK_REFERENCES_P(obj) FLAG_SET_P(obj, RefsAreWeakFlag)
#define HAS_WEAK_REFS_P(obj) FLAG_SET_P(obj, HasWeakRefsFlag)

static inline uint32_t object_get_id(STATE, OBJECT self) {
  if(REFERENCE_P(self)) {
    /* Lazy allocate object's ids, since most don't need them. */
    if(HEADER(self)->object_id == 0) {
      HEADER(self)->object_id = state->om->last_object_id++;
    }
    
    return HEADER(self)->object_id;
  } else {
    return (uint32_t)(self);
  }
}

#endif
