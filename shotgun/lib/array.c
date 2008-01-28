#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/object.h"
#include "shotgun/lib/tuple.h"

// TODO - Support >32bit counts?
OBJECT array_new(STATE, size_t count) {
  OBJECT tup, obj;
  tup = tuple_new(state, count);
  obj = array_allocate(state);
  array_set_total(obj, I2N(0));
  array_set_start(obj, I2N(0));
  array_set_tuple(obj, tup);
  return obj;
}

OBJECT array_from_tuple(STATE, OBJECT tuple) {
  OBJECT tup, ary;
  size_t count;
  count = NUM_FIELDS(tuple);
  tup = tuple_new(state, count);
  object_copy_fields(state, tuple, tup);
  ary = array_new(state, count);
  array_set_total(ary, I2N(count));
  array_set_tuple(ary, tup);
  return ary;
}

// TODO - Support >32bit counts?
OBJECT array_set(STATE, OBJECT self, size_t idx, OBJECT val) {
  OBJECT nt, tup;
  size_t cur, oidx;
  
  tup = array_get_tuple(self);
  cur = NUM_FIELDS(tup);
  
  oidx = idx;
  idx += N2I(array_get_start(self));
  
  if(idx >= cur) {
    size_t new_size = (cur == 0) ? 1 : cur;

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
  if(N2I(array_get_total(self)) <= oidx) {
    array_set_total(self, ML2N(oidx+1));
  }
  return val;
}

// TODO - Support >32bit counts?
OBJECT array_get(STATE, OBJECT self, size_t idx) {
  if(idx >= N2I(array_get_total(self))) {
    return Qnil;
  }
  
  idx += N2I(array_get_start(self));
  
  return tuple_at(state, array_get_tuple(self), idx);
}

OBJECT array_append(STATE, OBJECT self, OBJECT val) {
  size_t idx;
  idx = N2I(array_get_total(self));
  return array_set(state, self, idx, val);
}

OBJECT array_pop(STATE, OBJECT self) {
  size_t idx;
  OBJECT val;
  
  idx = N2I(array_get_total(self)) - 1;
  val = array_get(state, self, idx);
  array_set(state, self, idx, Qnil);
  
  array_set_total(self, ML2N(idx));
  return val;
}
