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
  object_copy_fields(state, tuple, tup);
  ary = array_new(state, count);
  array_set_total(ary, I2N(count));
  array_set_tuple(ary, tup);
  return ary;
}

OBJECT array_set(STATE, OBJECT self, int idx, OBJECT val) {
  OBJECT nt, tup;
  int cur;
  
  tup = array_get_tuple(self);
  cur = NUM_FIELDS(tup);
  
  if(idx >= cur) {
    int new_size = (cur == 0) ? 1 : cur;

    /* geometric expansion to fit idx in */
    while (new_size <= idx) {
      new_size *= 2;
    }

    nt = tuple_new(state, new_size);
    object_copy_fields_from(state, tup, nt, 0, cur);
    array_set_tuple(self, nt);
    tup = nt;
  }
  
  tuple_put(state, tup, idx, val);
  if(FIXNUM_TO_INT(array_get_total(self)) <= idx) {
    array_set_total(self, I2N(idx+1));
  }
  return val;
}

OBJECT array_get(STATE, OBJECT self, int idx) {
  if(idx >= FIXNUM_TO_INT(array_get_total(self))) {
    return Qnil;
  }
  return tuple_at(state, array_get_tuple(self), idx);
}

OBJECT array_append(STATE, OBJECT self, OBJECT val) {
  int idx;
  idx = FIXNUM_TO_INT(array_get_total(self));
  return array_set(state, self, idx, val);
}

OBJECT array_pop(STATE, OBJECT self) {
  int idx;
  OBJECT val;
  
  idx = FIXNUM_TO_INT(array_get_total(self)) - 1;
  val = array_get(state, self, idx);
  
  array_set_total(self, I2N(idx));
  return val;
}
