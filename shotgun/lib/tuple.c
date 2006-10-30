#include "shotgun.h"

OBJECT tuple_new(STATE, int size) {
  return tuple_allocate_with_extra(state, size);
}

OBJECT tuple_enlarge(STATE, OBJECT tup, int inc) {
  int sz;
  OBJECT ns;
  sz = NUM_FIELDS(tup);
  ns = tuple_new(state, sz + inc);
  object_copy_fields_from(state, tup, ns, 0, sz);
  return ns;
}
