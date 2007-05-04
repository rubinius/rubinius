#include "shotgun.h"
#include "object.h"
#include "tuple.h"

OBJECT array_new(STATE, int count) {
  OBJECT tup, obj;
  tup = tuple_new(state, count);
  obj = array_allocate(state);
  array_set_total(obj, I2N(0));
  array_set_tuple(obj, tup);
  return obj;
}

OBJECT array_from_tuple(STATE, OBJECT tuple) {
  OBJECT tup, ary;
  int count;
  count = NUM_FIELDS(tuple);
  tup = tuple_new(state, count);
  object_copy_fields_from(state, tuple, tup, 0, count);
  ary = array_new(state, count);
  array_set_total(ary, I2N(count));
  array_set_tuple(ary, tup);
  return ary;
}

OBJECT array_set(STATE, OBJECT self, int idx, OBJECT val) {
  OBJECT nt;
  int cur = NUM_FIELDS(array_get_tuple(self));
  if(idx == cur) {
    nt = tuple_new(state, cur + 10);
    object_copy_fields_from(state, array_get_tuple(self), nt, 0, cur);
    array_set_tuple(self, nt);
  }
  
  tuple_put(state, array_get_tuple(self), idx, val);
  if(FIXNUM_TO_INT(array_get_total(self)) <= idx) {
    array_set_total(self, I2N(idx+1));
  }
  return val;
}

OBJECT array_get(STATE, OBJECT self, int idx) {
  return tuple_at(state, array_get_tuple(self), idx);
}

OBJECT array_append(STATE, OBJECT self, OBJECT val) {
  int idx;
  idx = FIXNUM_TO_INT(array_get_total(self));
  return array_set(state, self, idx, val);
}

OBJECT array_pop(STATE, OBJECT self) {
  int idx;
  
  idx = FIXNUM_TO_INT(array_get_total(self)) - 1;
  array_set_total(self, I2N(idx));
  return array_get(state, self, idx);
}
