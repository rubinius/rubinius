#include "gc/gc.hpp"

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
    size_t bytes;
    Tuple* tup = state->om->new_object_variable<Tuple>(G(tuple), fields, bytes);
    if(unlikely(!tup)) {
      Exception::memory_error(state);
    }
    tup->full_size_ = bytes;
    return tup;
  }

  Tuple* Tuple::allocate(STATE, Fixnum* fields) {
    return Tuple::create(state, fields->to_native());
  }

  Tuple* Tuple::from(STATE, size_t fields, ...) {
    va_list ar;
    Tuple* tup = create(state, fields);

    va_start(ar, fields);
    for(size_t i = 0; i < fields; i++) {
      Object *obj = va_arg(ar, Object*);
      // fields equals size so bounds checking is unecessary
      tup->field[i] = obj;
      if(obj->reference_p()) tup->write_barrier(state, obj);
    }
    va_end(ar);

    return tup;
  }

  Tuple* Tuple::copy_from(STATE, Tuple* other, Fixnum* start, Fixnum *length, Fixnum* dest) {
    size_t osize = other->num_fields();
    size_t size = this->num_fields();

    int olend = start->to_native();
    int lend = dest->to_native();
    int olength = length->to_native();

    // left end should be within range
    if(olend < 0 || (size_t)olend > osize) {
      Exception::object_bounds_exceeded_error(state, other, olend);
    }

    if(lend < 0 || (size_t)lend > size) {
      Exception::object_bounds_exceeded_error(state, this, lend);
    }

    // length can not be negative and must fit in src/dest
    if(olength < 0) {
      Exception::object_bounds_exceeded_error(state, "length must be positive");
    }

    if((size_t)(olend + olength) > osize) {
      Exception::object_bounds_exceeded_error(state,
          "length should not exceed size of source");
    }

    if((size_t)olength > (size - lend)) {
      Exception::object_bounds_exceeded_error(state,
          "length should not exceed space in destination");
    }

    for(size_t src = olend, dst = lend;
        src < (size_t)(olend + olength);
        ++src, ++dst) {
      // Since we have carefully checked the bounds we don't need to do it in at/put
      Object *obj = other->field[src];
      this->field[dst] = obj;
      // but this is necessary to keep the GC happy
      if(obj->reference_p()) write_barrier(state, obj);
    }

    return this;
  }

  Fixnum* Tuple::delete_inplace(STATE, Fixnum *start, Fixnum *length, Object *obj) {
    int size = this->num_fields();
    int lend = start->to_native();
    int rend = lend + length->to_native();

    if(size == 0) return Fixnum::from(0);
    if(lend < 0 || lend >= size) {
      Exception::object_bounds_exceeded_error(state, this, lend);
    }

    if(rend < 0 || rend > size) {
      Exception::object_bounds_exceeded_error(state, this, rend);
    }

    int i = lend;
    while(i < rend) {
      if(this->at(state,i) == obj) {
        int j = i;
        ++i;
        while(i < rend) {
          Object *val = this->field[i];
          if(val != obj) {
            // no need to set write_barrier since it's already
            // referenced to this object
            this->field[j] = val;
            ++j;
          }
          ++i;
        }
        // cleanup all the bins after
        i = j;
        while(i < rend) {
          this->field[i] = Qnil;
          ++i;
        }
        return Fixnum::from(rend-j);
      }
      ++i;
    }
    return Fixnum::from(0);
  }

  /** @todo Add some error checking/handling and
   *  evaluate corner cases, and add tests... --rue
   */
  Tuple* Tuple::lshift_inplace(STATE, Fixnum* shift) {
    std::size_t size = this->num_fields();
    const int start = shift->to_native();

    if(start > 0) {
      std::size_t i = 0;
      std::size_t j = start;

      while(j < size) {
        this->field[i++] = this->field[j++];
      }

      while(i < size) {
        this->field[i++] = Qnil;
      }
    }

    return this;
  }

  Object* Tuple::reverse(STATE, Fixnum* o_start, Fixnum* o_total) {
    native_int start = o_start->to_native();
    native_int total = o_total->to_native();

    if(start < 0) return this;

    native_int limit = start + (total / 2);

    if(start > full_size_) return this;
    if(limit > full_size_) limit = full_size_ - 1;

    native_int i = start;
    native_int j = start + total - 1;

    Object** pos1 = field + i;
    Object** pos2 = field + j;

    register Object* tmp;
    while(pos1 < pos2) {
      tmp = *pos1;
      *pos1++ = *pos2;
      *pos2-- = tmp;
    }

    return this;
  }

  // @todo performance primitive; could be replaced with Ruby
  Tuple* Tuple::pattern(STATE, Fixnum* size, Object* val) {
    native_int cnt = size->to_native();
    Tuple* tuple = Tuple::create(state, cnt);

    // val is referend size times, we only need to hit the write
    // barrier once
    if(val->reference_p()) tuple->write_barrier(state, val);
    for(native_int i = 0; i < cnt; i++) {
      // bounds checking is covered because we instantiated the tuple
      // in this method
      tuple->field[i] = val;
    }

    return tuple;
  }

  size_t Tuple::Info::object_size(const ObjectHeader* obj) {
    return force_as<Tuple>(obj)->full_size_;
  }


  void Tuple::Info::mark(Object* obj, ObjectMark& mark) {
    Object* tmp;
    Tuple* tup = as<Tuple>(obj);

    for(size_t i = 0; i < tup->num_fields(); i++) {
      tmp = mark.call(tup->field[i]);
      if(tmp) mark.set(obj, &tup->field[i], tmp);
    }
  }

  void Tuple::Info::visit(Object* obj, ObjectVisitor& visit) {
    Tuple* tup = as<Tuple>(obj);

    for(size_t i = 0; i < tup->num_fields(); i++) {
      visit.call(tup->field[i]);
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
