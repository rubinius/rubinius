#include "builtin/tuple.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/compactlookuptable.hpp"

#include <cstdarg>
#include <iostream>

namespace rubinius {
  /* The Tuple#at primitive. */
  OBJECT Tuple::at_prim(STATE, FIXNUM index_obj) {
    return at(state, index_obj->to_native());
  }

  OBJECT Tuple::put(STATE, size_t idx, OBJECT val) {
    if(num_fields() <= idx) {
      Exception::object_bounds_exceeded_error(state, this, idx);
    }

    this->field[idx] = val;
    if(val->reference_p()) write_barrier(state, val);
    return val;
  }

  /* The Tuple#put primitive. */
  OBJECT Tuple::put_prim(STATE, FIXNUM index, OBJECT val) {
    return put(state, index->to_native(), val);
  }

  /* The Tuple#fields primitive. */
  OBJECT Tuple::fields_prim(STATE) {
    return Integer::from(state, num_fields());
  }

  Tuple* Tuple::create(STATE, size_t fields) {
    return (Tuple*)state->om->new_object(G(tuple), fields);
  }

  Tuple* Tuple::allocate(STATE, Fixnum* fields) {
    return Tuple::create(state, fields->to_native());
  }

  Tuple* Tuple::from(STATE, size_t fields, ...) {
    va_list ar;
    Tuple* tup = create(state, fields);

    va_start(ar, fields);
    for(size_t i = 0; i < fields; i++) {
      tup->put(state, i, va_arg(ar, OBJECT));
    }
    va_end(ar);

    return tup;
  }

  /*
   * Copies items from +other+ to this tuple from +start+ to +end+ inclusive.
   */
  void Tuple::replace_with(STATE, Tuple* other, int start, int end) {
    size_t length;

    if(start < 0) start = 0;
    if((size_t)end >= other->num_fields()) {
      length = other->num_fields() - 1;
    } else {
      length = end;
    }
    if(length - start + 1 > this->num_fields()) {
      length = start + this->num_fields() - 1;
    }

    for(size_t i = start, j = 0; i <= length; i++, j++) {
      this->put(state, j, other->at(state, i));
    }
  }

  Tuple* Tuple::shifted(STATE, FIXNUM num) {
    native_int cnt = num->to_native();

    if(cnt == 0) {
      return this;
    } else {
      Tuple* tuple = Tuple::create(state, this->num_fields() + cnt);

      for(size_t i = 0; i < this->num_fields(); i++) {
        tuple->put(state, i + cnt, this->at(state, i));
      }

      return tuple;
    }
  }

  Tuple* Tuple::copy_from(STATE, Tuple* other, FIXNUM start, FIXNUM dest) {
    native_int src = start->to_native();
    native_int dst = dest->to_native();
    native_int sz = this->num_fields();
    native_int osz = other->num_fields();
    native_int i, j;

    if(src < 0) src = 0;
    if(dst < 0) dst = 0;
    for(i = src, j = dst; i < osz && j < sz; i++, j++) {
      this->put(state, j, other->at(state, i));
    }

    return this;
  }

  // TODO: performance primitive; could be replaced with Ruby
  Tuple* Tuple::pattern(STATE, FIXNUM size, OBJECT val) {
    native_int cnt = size->to_native();
    Tuple* tuple = Tuple::create(state, cnt);

    for(native_int i = 0; i < cnt; i++) {
      tuple->put(state, i, val);
    }

    return tuple;
  }

  Tuple* Tuple::create_weakref(STATE, OBJECT obj) {
    Tuple* tup = Tuple::from(state, 1, obj);
    tup->RefsAreWeak = 1;
    return tup;
  }

  void Tuple::Info::mark(OBJECT obj, ObjectMark& mark) {
    OBJECT tmp;
    Tuple* tup = as<Tuple>(obj);

    for(size_t i = 0; i < tup->num_fields(); i++) {
      tmp = mark.call(tup->field[i]);
      if(tmp) mark.set(obj, &tup->field[i], tmp);
    }
  }

  void Tuple::Info::show(STATE, OBJECT self, int level) {
    Tuple* tup = as<Tuple>(self);
    size_t size = tup->num_fields();
    size_t stop = size < 6 ? size : 6;

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    ++level;
    for(size_t i = 0; i < stop; i++) {
      indent(level);
      OBJECT obj = tup->at(state, i);
      if(obj == tup) {
        class_info(state, self, true);
      } else {
        obj->show(state, level);
      }
    }
    if(tup->num_fields() > stop) ellipsis(level);
    close_body(level);
  }

  void Tuple::Info::show_simple(STATE, OBJECT self, int level) {
    Tuple* tup = as<Tuple>(self);
    size_t size = tup->num_fields();
    size_t stop = size < 6 ? size : 6;

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    ++level;
    for(size_t i = 0; i < stop; i++) {
      indent(level);
      OBJECT obj = tup->at(state, i);
      if(Tuple* t = try_as<Tuple>(obj)) {
        class_info(state, self);
        std::cout << ": " << t->num_fields() << ">" << std::endl;
      } else {
        obj->show_simple(state, level);
      }
    }
    if(tup->num_fields() > stop) ellipsis(level);
    close_body(level);
  }
}
