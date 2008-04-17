#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

namespace rubinius {

  Array* Array::create(STATE, size_t idx) {
    Array* ary;
    ary = (Array*)state->om->new_object(state->globals.array, Array::fields);

    ary->setup(state, idx);

    return ary;
  }

  Array* Array::from_tuple(STATE, Tuple* tup) {

  }

  void Array::setup(STATE, size_t size) {
    SET(this, tuple, Tuple::create(state, size));
    SET(this, start, Object::i2n(0));
    SET(this, total, Object::i2n(0));
  }

  OBJECT Array::get(STATE, size_t idx) {
    if(idx >= (size_t)total->n2i()) {
      return Qnil;
    }

    idx += start->n2i();

    return tuple->at(idx);
  }

  OBJECT Array::set(STATE, size_t idx, OBJECT val) {
    size_t cur, oidx;

    Tuple* tup = tuple;
    cur = tup->field_count;

    oidx = idx;
    idx += start->n2i();

    if(idx >= cur) {
      size_t new_size = (cur == 0) ? 1 : cur;

      /* geometric expansion to fit idx in */
      while (new_size <= idx) {
        new_size *= 2;
      }

      Tuple* nt = Tuple::create(state, new_size);
      for(size_t i = 0; i < cur; i++) {
        nt->put(state, i, tup->at(i));
      }

      SET(this, tuple, nt);
      tup = nt;
    }

    tup->put(state, idx, val);
    if((size_t)total->n2i() <= oidx) {
      total = Object::i2n(oidx+1);
    }
    return val;
  }

  OBJECT Array::append(STATE, OBJECT val) {
    set(state, (size_t)total->n2i(), val);
    return val;
  }

  bool Array::includes_p(STATE, OBJECT val) {
    size_t max = size();

    for(size_t i = 0; i < max; i++) {
      if(tuple->at(i) == val) return true;
    }

    return false;
  }
}
