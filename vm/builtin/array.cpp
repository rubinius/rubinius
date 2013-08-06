#include "arguments.hpp"
#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"
#include "configuration.hpp"
#include "dispatch.hpp"
#include "object_utils.hpp"
#include "ontology.hpp"
#include "version.h"

/* Implementation certain Array methods. These methods are just
 * the ones the VM requires, not the entire set of all Array methods.
 * This includes methods required to implement certain Array
 * primitives. */

namespace rubinius {

  native_int Array::size() {
    return total_->to_native();
  }

  void Array::set_size(native_int size) {
    total_ = Fixnum::from(size);
  }

  native_int Array::offset() {
    return start_->to_native();
  }

  Array* Array::create(STATE, native_int size) {
    Array* ary;
    ary = state->new_object_dirty<Array>(G(array));

    ary->setup(state, size);

    return ary;
  }

  // 'self' is passed in automatically by the primitive glue
  Array* Array::allocate(STATE, Object* self) {
    Array* ary = Array::create(state, 0);
    ary->klass(state, as<Class>(self));
    return ary;
  }

  Array* Array::new_range(STATE, Fixnum* start, Fixnum* count) {
    Array* ary = state->new_object_dirty<Array>(class_object(state));
    ary->total(state, count);
    ary->start(state, Fixnum::from(0));

    native_int total = count->to_native();
    if(total <= 0) {
      ary->total(state, Fixnum::from(0));
      ary->tuple(state, Tuple::create(state, 0));
    } else {
      Tuple* tup = Tuple::create(state, total);
      Tuple* orig = tuple_;

      for(native_int i = 0, j = start->to_native(); i < total; i++, j++) {
        tup->put(state, i, orig->at(state, j));
      }

      ary->tuple(state, tup);
    }
    return ary;
  }

  Array* Array::new_reserved(STATE, Fixnum* count) {
    Array* ary = state->new_object_dirty<Array>(class_object(state));
    ary->start(state, Fixnum::from(0));
    ary->total(state, Fixnum::from(0));

    native_int total = count->to_native();
    if(total <= 0) total = 1;
    ary->tuple(state, Tuple::create(state, total));

    return ary;
  }

  Array* Array::from_tuple(STATE, Tuple* tup) {
    native_int length = tup->num_fields();
    Array* ary = Array::create(state, length);
    ary->tuple_->copy_from(state, tup,
        Fixnum::from(0), Fixnum::from(length),
        Fixnum::from(0));

    ary->total(state, Fixnum::from(length));
    return ary;
  }

  Array* Array::to_ary(STATE, Object* value, CallFrame* call_frame) {
    if(Tuple* tup = try_as<Tuple>(value)) {
      return Array::from_tuple(state, tup);
    }

    if(CBOOL(value->respond_to(state, G(sym_to_ary), cTrue))) {
      Object* res = value->send(state, call_frame, G(sym_to_ary));
      if(!res) return 0;

      if(Array* ary = try_as<Array>(res)) {
        return ary;
      }

      if(LANGUAGE_18_ENABLED || !res->nil_p()) {
        Exception::type_error(state, "to_ary should return an Array", call_frame);
        return 0;
      }

      // NOTE: On >= 1.9, if res is nil just fall through and return [value]
    }

    Array* ary = Array::create(state, 1);
    ary->set(state, 0, value);
    return ary;
  }

  void Array::setup(STATE, native_int size) {
    this->tuple(state, Tuple::create(state, size));
    this->start(state, Fixnum::from(0));
    this->total(state, Fixnum::from(0));
  }

  // NOTE: We don't use Primitives::failure() here because the wrapper
  // code makes sure we're only called when the arity and type are correct.
  // Thus we know that this is a simple a[n] case only, which we can
  // fully handle.
  Object* Array::aref(STATE, Fixnum* idx) {
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
    if(index >= total || index < start) return cNil;

    return tuple_->at(state, index);
  }

  Object* Array::aset(STATE, Fixnum* idx, Object* val) {
    if(is_frozen_p()) return Primitives::failure();

    native_int index = idx->to_native();

    if(index < 0) {
      index += total_->to_native();
      if(index < 0) return Primitives::failure();
    }

    return this->set(state, index, val);
  }

  Array* Array::concat(STATE, Array* other) {
    if(!LANGUAGE_18_ENABLED) {
      if(is_frozen_p()) return force_as<Array>(Primitives::failure());
    }

    native_int osize = other->size();

    if(osize == 0) return this;
    if(LANGUAGE_18_ENABLED) {
      if(is_frozen_p()) return force_as<Array>(Primitives::failure());
    }

    if(osize == 1) {
      set(state, size(), other->get(state, 0));
      return this;
    }

    native_int new_size = size() + osize;
    Tuple* nt = Tuple::create(state, new_size);
    nt->copy_from(state, tuple_, start_, total_, Fixnum::from(0));
    nt->copy_from(state, other->tuple(), other->start(), other->total(), total_);

    tuple(state, nt);
    start(state, Fixnum::from(0));
    total(state, Fixnum::from(new_size));

    return this;
  }

  Object* Array::get(STATE, native_int idx) {
    if(idx >= total_->to_native()) {
      return cNil;
    }

    idx += start_->to_native();

    return tuple_->at(state, idx);
  }

  Object* Array::set(STATE, native_int idx, Object* val) {
    native_int tuple_size = tuple_->num_fields();
    native_int oidx = idx;
    idx += start_->to_native();

    if(idx >= tuple_size) {
      if(oidx < tuple_size) {
        // There is enough space in the tuple for this element
        tuple_->lshift_inplace(state, start_);
      } else {
        // Uses the same algo as 1.8 to resize the tuple
        native_int new_size = tuple_size / 2;
        if(new_size < 3) {
          new_size = 3;
        }

        Tuple* nt = Tuple::create(state, new_size+idx);
        nt->copy_from(state, tuple_, start_, total_, Fixnum::from(0));
        tuple(state, nt);
      }
      start(state, Fixnum::from(0));
      idx = oidx;
    }

    tuple_->put(state, idx, val);
    if(total_->to_native() <= oidx) {
      total(state, Fixnum::from(oidx+1));
    }
    return val;
  }

  void Array::unshift(STATE, Object* val) {
    native_int new_size = total_->to_native() + 1;
    native_int lend = start_->to_native();

    if(lend > 0) {
      tuple_->put(state, lend-1, val);
      start(state, Fixnum::from(lend-1));
      total(state, Fixnum::from(new_size));
    } else {
      Tuple* nt = Tuple::create(state, new_size);
      nt->copy_from(state, tuple_, start_, total_,
		    Fixnum::from(1));
      nt->put(state, 0, val);

      total(state, Fixnum::from(new_size));
      start(state, Fixnum::from(0));
      tuple(state, nt);
    }
  }

  Object* Array::shift(STATE) {
    native_int cnt = total_->to_native();

    if(cnt == 0) return cNil;
    Object* obj = get(state, 0);
    set(state, 0, cNil);
    start(state, Fixnum::from(start_->to_native() + 1));
    total(state, Fixnum::from(cnt - 1));
    return obj;
  }

  Object* Array::append(STATE, Object* val) {
    set(state, total_->to_native(), val);
    return val;
  }

  bool Array::includes_p(STATE, Object* val) {
    native_int cnt = total_->to_native();

    for(native_int i = 0; i < cnt; i++) {
      if(get(state, i) == val) return true;
    }

    return false;
  }

  Object* Array::pop(STATE) {
    native_int cnt = total_->to_native();

    if(cnt == 0) return cNil;
    Object *obj = get(state, cnt - 1);
    set(state, cnt-1, cNil);
    total(state, Fixnum::from(cnt - 1));
    return obj;
  }

  void Array::Info::show(STATE, Object* self, int level) {
    Array* ary = as<Array>(self);
    native_int size = ary->size();
    native_int stop = size < 5 ? size : 5;

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    ++level;
    for(native_int i = 0; i < stop; i++) {
      indent(level);
      Object* obj = ary->get(state, i);
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
