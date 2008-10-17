#include "vm.hpp"
#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"
#include "builtin/class.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"

#include <iostream>

/* Implementation certain Array methods. These methods are just
 * the ones the VM requires, not the entire set of all Array methods.
 * This includes methods required to implement certain Array
 * primitives. */

namespace rubinius {

  void Array::init(STATE) {
    GO(array).set(state->new_class("Array", G(object), Array::fields));
    G(array)->set_object_type(state, ArrayType);
  }

  size_t Array::size() {
    return total_->to_native();
  }

  Array* Array::create(STATE, size_t idx) {
    Array* ary;
    ary = (Array*)state->om->new_object(G(array), Array::fields);

    ary->setup(state, idx);

    return ary;
  }

  Array* Array::from_tuple(STATE, Tuple* tup) {
    Array* ary = Array::create(state, tup->num_fields());
    for(size_t i = 0; i < tup->num_fields(); i++) {
      ary->set(state, i, tup->at(state, i));
    }

    return ary;
  }

  void Array::setup(STATE, size_t size) {
    this->tuple(state, Tuple::create(state, size));
    this->start(state, Fixnum::from(0));
    this->total(state, Fixnum::from(0));
  }

  // NOTE: We don't use Primitives::failure() here because the wrapper
  // code makes sure we're only called when the arity and type are correct.
  // Thus we know that this is a simple a[n] case only, which we can
  // fully handle.
  OBJECT Array::aref(STATE, Fixnum* idx) {
    native_int index = idx->to_native();
    const native_int start = start_->to_native();
    const native_int total = start + total_->to_native();

    // Handle negative indexes
    if(index < 0) {
      index += total;
    } else {
      index += start;
    }

    // Off either end, return nil
    if(index >= total || index < start) return Qnil;

    return tuple_->at(state, index);
  }

  OBJECT Array::aset(STATE, Fixnum* idx, OBJECT val) {
    native_int index = idx->to_native();

    if(index < 0) {
      return Primitives::failure();
    }

    return this->set(state, index, val);
  }

  OBJECT Array::get(STATE, size_t idx) {
    if(idx >= (size_t)total_->to_native()) {
      return Qnil;
    }

    idx += start_->to_native();

    return tuple_->at(state, idx);
  }

  OBJECT Array::set(STATE, size_t idx, OBJECT val) {
    size_t cur, oidx;

    Tuple* tup = tuple_;
    cur = tup->num_fields();

    oidx = idx;
    idx += start_->to_native();

    if(idx >= cur) {
      // Uses the same algo as 1.8 to resize the tuple
      size_t new_size = cur / 2;
      if(new_size < 3) {
        new_size = 3;
      }

      new_size += idx;
      Tuple* nt = Tuple::create(state, new_size);
      for(size_t i = 0; i < cur; i++) {
        nt->put(state, i, tup->at(state, i));
      }

      tuple(state, nt);
      tup = nt;
    }

    tup->put(state, idx, val);
    if((size_t)total_->to_native() <= oidx) {
      total(state, Fixnum::from(oidx+1));
    }
    return val;
  }

  void Array::unshift(STATE, OBJECT val) {
    size_t new_size = total_->to_native() + 1;
    Tuple* nt = Tuple::create(state, new_size);
    for(size_t i = 0; i < (size_t)total_->to_native(); i++) {
      nt->put(state, i + 1, get(state, i));
    }

    nt->put(state, 0, val);
    total(state, Fixnum::from(new_size));
    start(state, Fixnum::from(0));

    tuple(state, nt);
  }

  OBJECT Array::shift(STATE) {
    OBJECT obj = get(state, 0);
    start(state, Fixnum::from(start_->to_native() + 1));
    total(state, Fixnum::from(total_->to_native() - 1));
    return obj;
  }

  OBJECT Array::append(STATE, OBJECT val) {
    set(state, (size_t)total_->to_native(), val);
    return val;
  }

  bool Array::includes_p(STATE, OBJECT val) {
    size_t max = size();

    for(size_t i = 0; i < max; i++) {
      if(tuple_->at(state, i) == val) return true;
    }

    return false;
  }

  void Array::Info::show(STATE, OBJECT self, int level) {
    Array* ary = as<Array>(self);
    size_t size = ary->size();
    size_t stop = size < 5 ? size : 5;

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    ++level;
    for(size_t i = 0; i < stop; i++) {
      indent(level);
      OBJECT obj = ary->get(state, i);
      if(obj == ary) {
        class_info(state, obj, true);
      } else {
        obj->show(state, level);
      }
    }
    if(ary->size() > stop) ellipsis(level);
    close_body(level);
  }
}
