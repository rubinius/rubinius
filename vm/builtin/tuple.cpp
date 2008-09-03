#include "builtin/tuple.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/fixnum.hpp"

#include <cstdarg>
#include <iostream>

namespace rubinius {
  OBJECT Tuple::at(size_t index) {
    if(field_count <= index) {
      ObjectBoundsExceeded::raise(this, index);
    }
    return field[index];
  }

  /* The Tuple#at primitive. */
  OBJECT Tuple::at_prim(STATE, FIXNUM index_obj) {
    size_t index = index_obj->to_native();
    if(field_count <= index) return Qnil;

    return field[index];
  }

  OBJECT Tuple::put(STATE, size_t idx, OBJECT val) {
    // store_object performs bounds check
    state->om->store_object(this, idx, val);
    return val;
  }

  /* The Tuple#put primitive. */
  OBJECT Tuple::put_prim(STATE, FIXNUM index, OBJECT val) {
    return put(state, index->to_native(), val);
  }

  /* The Tuple#fields primitive. */
  OBJECT Tuple::fields_prim(STATE) {
    return Integer::from(state, field_count);
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
    if((size_t)end >= other->field_count) {
      length = other->field_count - 1;
    } else {
      length = end;
    }
    if(length - start + 1 > this->field_count) {
      length = start + this->field_count - 1;
    }

    for(size_t i = start, j = 0; i <= length; i++, j++) {
      this->put(state, j, other->at(i));
    }
  }

  Tuple* Tuple::shifted(STATE, FIXNUM num) {
    native_int cnt = num->to_native();

    if(cnt == 0) {
      return this;
    } else {
      Tuple* tuple = Tuple::create(state, this->field_count + cnt);

      for(size_t i = 0; i < this->field_count; i++) {
        tuple->put(state, i + cnt, this->at(i));
      }

      return tuple;
    }
  }

  Tuple* Tuple::copy_from(STATE, Tuple* other, FIXNUM start, FIXNUM dest) {
    native_int src = start->to_native();
    native_int dst = dest->to_native();
    native_int sz = this->field_count;
    native_int osz = other->field_count;
    native_int i, j;

    if(src < 0) src = 0;
    if(dst < 0) dst = 0;
    for(i = src, j = dst; i < osz && j < sz; i++, j++) {
      this->put(state, j, other->at(i));
    }

    return this;
  }

  Tuple* Tuple::create_weakref(STATE, OBJECT obj) {
    Tuple* tup = Tuple::from(state, 1, obj);
    tup->RefsAreWeak = 1;
    return tup;
  }

  void Tuple::Info::mark(OBJECT obj, ObjectMark& mark) {
    OBJECT tmp;
    Tuple* tup = as<Tuple>(obj);

    for(size_t i = 0; i < tup->field_count; i++) {
      tmp = mark.call(tup->field[i]);
      if(tmp) mark.set(obj, &tup->field[i], tmp);
    }
  }

  void Tuple::Info::show(STATE, OBJECT self, int level) {
    Tuple* tup = as<Tuple>(self);
    size_t size = tup->field_count;
    size_t stop = size < 5 ? size : 5;

    class_info(self);
    std::cout << ": " << size << "\n";
    ++level;
    for(size_t i = 0; i < stop; i++) {
      indent(level);
      OBJECT obj = tup->at(i);
      if(obj == tup) {
        class_info(self, true);
      } else {
        obj->show(state, level);
      }
    }
    if(tup->field_count > stop) {
      indent(level);
      std::cout << "...\n";
    }
    indent(--level); std::cout << ">\n";
  }
}
