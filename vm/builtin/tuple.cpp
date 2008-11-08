#include "builtin/tuple.hpp"
#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/compactlookuptable.hpp"

#include <cstdarg>
#include <iostream>

namespace rubinius {
  /* The Tuple#at primitive. */
  Object* Tuple::at_prim(STATE, Fixnum* index_obj) {
    return at(state, index_obj->to_native());
  }

  Object* Tuple::put(STATE, size_t idx, Object* val) {
    if(num_fields() <= idx) {
      Exception::object_bounds_exceeded_error(state, this, idx);
    }

    this->field[idx] = val;
    if(val->reference_p()) write_barrier(state, val);
    return val;
  }

  /* The Tuple#put primitive. */
  Object* Tuple::put_prim(STATE, Fixnum* index, Object* val) {
    return put(state, index->to_native(), val);
  }

  /* The Tuple#fields primitive. */
  Object* Tuple::fields_prim(STATE) {
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
      tup->put(state, i, va_arg(ar, Object*));
    }
    va_end(ar);

    return tup;
  }

  /*
   * Copies items from +other+ to this tuple from +start+ to +end+ inclusive.
   */
  void Tuple::copy_range(STATE, Tuple* other, int start, int end, int dest) {
    size_t sz = this->num_fields();
    size_t osz = other->num_fields();

    if(osz < 1) return;
    if(start < 0) start = 0;
    if(dest < 0) dest = 0;
    size_t length = (size_t)end >= osz ? osz - 1 : end;
    
    for(size_t i = start, j = dest; i <= length && j < sz; ++i, ++j) {
      this->put(state, j, other->at(state, i));
    }
  }

  Tuple* Tuple::shifted(STATE, Fixnum* num) {
    native_int cnt = num->to_native();

    if(cnt == 0) {
      return this;
    } else {
      Tuple* tuple = Tuple::create(state, this->num_fields() + cnt);
      tuple->copy_range(state, this, 0, this->num_fields() - 1, cnt); 
      return tuple;
    }
  }

  Tuple* Tuple::copy_range(STATE, Tuple* other, Fixnum* start, Fixnum *end, Fixnum* dest) {
    copy_range(state,other,start->to_native(),end->to_native(),dest->to_native());
    
    return this;
  }

  Tuple* Tuple::copy_from(STATE, Tuple* other, Fixnum* start, Fixnum* dest) {
    copy_range(state,other,start->to_native(),other->num_fields()-1,dest->to_native());

    return this;
  }

  // @todo performance primitive; could be replaced with Ruby
  Tuple* Tuple::pattern(STATE, Fixnum* size, Object* val) {
    native_int cnt = size->to_native();
    Tuple* tuple = Tuple::create(state, cnt);

    for(native_int i = 0; i < cnt; i++) {
      tuple->put(state, i, val);
    }

    return tuple;
  }

  Tuple* Tuple::create_weakref(STATE, Object* obj) {
    Tuple* tup = Tuple::from(state, 1, obj);
    tup->RefsAreWeak = 1;
    return tup;
  }

  void Tuple::Info::mark(Object* obj, ObjectMark& mark) {
    Object* tmp;
    Tuple* tup = as<Tuple>(obj);

    for(size_t i = 0; i < tup->num_fields(); i++) {
      tmp = mark.call(tup->field[i]);
      if(tmp) mark.set(obj, &tup->field[i], tmp);
    }
  }

  void Tuple::Info::show(STATE, Object* self, int level) {
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
      Object* obj = tup->at(state, i);
      if(obj == tup) {
        class_info(state, self, true);
      } else {
        obj->show(state, level);
      }
    }
    if(tup->num_fields() > stop) ellipsis(level);
    close_body(level);
  }

  void Tuple::Info::show_simple(STATE, Object* self, int level) {
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
      Object* obj = tup->at(state, i);
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
