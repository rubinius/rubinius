#include "vm.hpp"
#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"
#include "builtin/class.hpp"
#include "objectmemory.hpp"

/* Implementation certain Array methods. These methods are just
 * the ones the VM requires, not the entire set of all Array methods.
 * This includes methods required to implement certain Array
 * primitives. */

namespace rubinius {

  void Array::init(STATE) {
    GO(array).set(state->new_class("Array", G(object), Array::fields));
    G(array)->set_object_type(ArrayType);
  }

  size_t Array::size() {
    return total->to_native();
  }

  Array* Array::create(STATE, size_t idx) {
    Array* ary;
    ary = (Array*)state->om->new_object(G(array), Array::fields);

    ary->setup(state, idx);

    return ary;
  }

  Array* Array::from_tuple(STATE, Tuple* tup) {
    Array* ary = Array::create(state, tup->field_count);
    OBJECT i = Fixnum::from(0);
    as<Integer>(i);
    for(size_t i = 0; i < tup->field_count; i++) {
      ary->set(state, i, tup->at(i));
    }

    return ary;
  }

  void Array::setup(STATE, size_t size) {
    SET(this, tuple, Tuple::create(state, size));
    SET(this, start, Fixnum::from(0));
    SET(this, total, Fixnum::from(0));
  }

  OBJECT Array::aref(STATE, Fixnum* idx) {
    return this->get(state, idx->to_native());
  }

  OBJECT Array::aset(STATE, Fixnum* idx, OBJECT val) {
    return this->set(state, idx->to_native(), val);
  }

  OBJECT Array::get(STATE, size_t idx) {
    if(idx >= (size_t)total->to_native()) {
      return Qnil;
    }

    idx += start->to_native();

    return tuple->at(idx);
  }

  OBJECT Array::set(STATE, size_t idx, OBJECT val) {
    size_t cur, oidx;

    Tuple* tup = tuple;
    cur = tup->field_count;

    oidx = idx;
    idx += start->to_native();

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
    if((size_t)total->to_native() <= oidx) {
      total = Fixnum::from(oidx+1);
    }
    return val;
  }

  void Array::unshift(STATE, OBJECT val) {
    size_t new_size = total->to_native() + 1;
    Tuple* nt = Tuple::create(state, new_size);
    for(size_t i = 0; i < (size_t)total->to_native(); i++) {
      nt->put(state, i + 1, get(state, i));
    }

    nt->put(state, 0, val);
    total = Fixnum::from(new_size);
    start = Fixnum::from(0);

    tuple = nt;
  }

  OBJECT Array::append(STATE, OBJECT val) {
    set(state, (size_t)total->to_native(), val);
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
