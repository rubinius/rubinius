#include "shotgun.h"
#include "object.h"
#include "tuple.h"

OBJECT array_new(STATE, int count) {
  OBJECT tup, obj;
  tup = tuple_new(state, count);
  obj = array_allocate(state);
  array_set_total(obj, I2N(count));
  array_set_tuple(obj, tup);
  return obj;
}

OBJECT array_from_tuple(STATE, OBJECT tuple) {
  OBJECT tup, ary;
  int count;
  count = NUM_FIELDS(tuple);
  tup = tuple_new(state, count);
  object_copy_fields_from(state, tuple, tup, count, 0);
  ary = array_new(state, count);
  array_set_total(ary, I2N(count));
  array_set_tuple(ary, tup);
  return ary;
}

OBJECT array_set(STATE, OBJECT self, int idx, OBJECT val) {
  tuple_put(state, array_get_tuple(self), idx, val);
  return val;
}

OBJECT array_get(STATE, OBJECT self, int idx) {
  return tuple_at(state, array_get_tuple(self), idx);
}
