#include "gc/gc.hpp"

#include "builtin/tuple.hpp"
#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/compactlookuptable.hpp"

#include <stdarg.h>
#include <iostream>
#include <stdint.h>

namespace rubinius {
  Tuple* Tuple::bounds_exceeded_error(STATE, const char* method, int index) {
    std::ostringstream msg;

    msg << method;
    msg << ": index " << index << " out of bounds for size " << num_fields();

    Exception::object_bounds_exceeded_error(state, msg.str().c_str());
    return 0;
  }

  /* The Tuple#at primitive. */
  Object* Tuple::at_prim(STATE, Fixnum* index_obj) {
    native_int index = index_obj->to_native();

    if(index < 0 || num_fields() <= index) {
      return bounds_exceeded_error(state, "Tuple::at_prim", index);
    }

    return field[index];
  }

  Object* Tuple::put(STATE, native_int idx, Object* val) {
    if(idx < 0 || idx >= num_fields()) {
      rubinius::bug("Invalid tuple index");
    }

    this->field[idx] = val;
    if(val->reference_p()) write_barrier(state, val);
    return val;
  }

  /* The Tuple#put primitive. */
  Object* Tuple::put_prim(STATE, Fixnum* index, Object* val) {
    native_int idx = index->to_native();

    if(idx < 0 || num_fields() <= idx) {
      return bounds_exceeded_error(state, "Tuple::put_prim", idx);
    }

    this->field[idx] = val;
    if(val->reference_p()) write_barrier(state, val);
    return val;
  }

  /* The Tuple#fields primitive. */
  Object* Tuple::fields_prim(STATE) {
    return Integer::from(state, num_fields());
  }

  Tuple* Tuple::create(STATE, native_int fields) {
    if(fields < 0 || fields >= INT32_MAX) {
      rubinius::bug("Invalid tuple size");
    }

    // Fast path using GC optimized tuple creation
    Tuple* tup = state->vm()->new_young_tuple_dirty(fields);

    if(likely(tup)) {
      for(native_int i = 0; i < fields; i++) {
        tup->field[i] = cNil;
      }

      return tup;
    }

    // Slow path.

    size_t bytes = 0;

    tup = state->vm()->new_object_variable<Tuple>(G(tuple), fields, bytes);
    if(unlikely(!tup)) {
      Exception::memory_error(state);
    } else {
      tup->full_size_ = bytes;
    }
    return tup;
  }

  Tuple* Tuple::allocate(STATE, Fixnum* fields) {
    native_int size = fields->to_native();

    if(size < 0) {
      Exception::argument_error(state, "negative tuple size");
    } else if(size > INT32_MAX) {
      Exception::argument_error(state, "too large tuple size");
    }

    return create(state, fields->to_native());
  }

  Tuple* Tuple::from(STATE, native_int fields, ...) {
    if(fields < 0 || fields >= INT32_MAX) {
      rubinius::bug("Invalid tuple size");
    }

    va_list ar;
    Tuple* tup = create(state, fields);

    va_start(ar, fields);
    for(native_int i = 0; i < fields; i++) {
      Object *obj = va_arg(ar, Object*);
      // fields equals size so bounds checking is unecessary
      tup->field[i] = obj;
      if(obj->reference_p()) tup->write_barrier(state, obj);
    }
    va_end(ar);

    return tup;
  }

  Tuple* Tuple::copy_from(STATE, Tuple* other, Fixnum* start, Fixnum *length, Fixnum* dest) {
    native_int osize = other->num_fields();
    native_int size = this->num_fields();

    int src_start = start->to_native();
    int dst_start = dest->to_native();
    int len = length->to_native();

    // left end should be within range
    if(src_start < 0 || src_start > osize) {
      return other->bounds_exceeded_error(state, "Tuple::copy_from", src_start);
    }

    if(dst_start < 0 || dst_start > size) {
      return bounds_exceeded_error(state, "Tuple::copy_from", dst_start);
    }

    // length can not be negative and must fit in src/dest
    if(len < 0) {
      return other->bounds_exceeded_error(state, "Tuple::copy_from", len);
    }

    if((src_start + len) > osize) {
      return other->bounds_exceeded_error(state, "Tuple::copy_from", src_start + len);
    }

    if(len > (size - dst_start)) {
      return bounds_exceeded_error(state, "Tuple::copy_from", len);
    }

    // A memmove within the tuple
    if(other == this) {
      // No movement, no work!
      if(src_start == dst_start) return this;
      // right shift
      if(src_start < dst_start) {
        for(native_int dest_idx = dst_start + len - 1,
                       src_idx  = src_start + len - 1;
            src_idx >= src_start;
            src_idx--, dest_idx--) {
          this->field[dest_idx] = this->field[src_idx];
        }
      } else {
        // left shift
        for(native_int dest_idx = dst_start,
                       src_idx  = src_start;
            src_idx < src_start + len;
            src_idx++, dest_idx++) {
          this->field[dest_idx] = this->field[src_idx];
        }
      }

    } else {
      for(native_int src = src_start, dst = dst_start;
          src < (src_start + len);
          ++src, ++dst) {
        // Since we have carefully checked the bounds we don't need
        // to do it in at/put
        Object *obj = other->field[src];
        this->field[dst] = obj;
        // but this is necessary to keep the GC happy
        if(obj->reference_p()) write_barrier(state, obj);
      }
    }

    return this;
  }

  Fixnum* Tuple::delete_inplace(STATE, Fixnum *start, Fixnum *length, Object *obj) {
    int size = this->num_fields();
    int len  = length->to_native();
    int lend = start->to_native();
    int rend = lend + len;

    if(size == 0 || len == 0) return Fixnum::from(0);
    if(lend < 0 || lend >= size) {
      return force_as<Fixnum>(bounds_exceeded_error(state, "Tuple::delete_inplace", lend));
    }

    if(rend < 0 || rend > size) {
      return force_as<Fixnum>(bounds_exceeded_error(state, "Tuple::delete_inplace", rend));
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
          this->field[i] = cNil;
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
    native_int size = this->num_fields();
    const int start = shift->to_native();

    assert(start >= 0);

    if(start > 0) {
      native_int i = 0;
      native_int j = start;

      while(j < size) {
        this->field[i++] = this->field[j++];
      }

      while(i < size) {
        this->field[i++] = cNil;
      }
    }

    return this;
  }

  Object* Tuple::reverse(STATE, Fixnum* o_start, Fixnum* o_total) {
    native_int start = o_start->to_native();
    native_int total = o_total->to_native();

    if(total <= 0 || start < 0 || start >= num_fields()) return this;

    native_int end = start + total - 1;
    if(end >= num_fields()) end = num_fields() - 1;

    Object** pos1 = field + start;
    Object** pos2 = field + end;

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

    if(cnt < 0) {
      Exception::argument_error(state, "negative tuple size");
    } else if (cnt > INT32_MAX) {
      Exception::argument_error(state, "too large tuple size");
    }

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

  Tuple* Tuple::tuple_dup(STATE) {
    native_int fields = num_fields();

    Tuple* tup = state->vm()->new_young_tuple_dirty(fields);

    if(likely(tup)) {
      for(native_int i = 0; i < fields; i++) {
        Object *obj = field[i];

        // fields equals size so bounds checking is unecessary
        tup->field[i] = obj;

        // Because tup is promised to be a young object,
        // we can elide the write barrier usage.
      }

      return tup;
    }

    // Otherwise, use slower creation path that might create
    // a mature object.
    tup = create(state, fields);

    for(native_int i = 0; i < fields; i++) {
      Object *obj = field[i];

      // fields equals size so bounds checking is unecessary
      tup->field[i] = obj;
      if(obj->reference_p()) tup->write_barrier(state, obj);
    }

    return tup;
  }

  size_t Tuple::Info::object_size(const ObjectHeader* obj) {
    return force_as<Tuple>(obj)->full_size_;
  }

  void Tuple::Info::mark(Object* obj, ObjectMark& mark) {
    Object* tmp;
    Tuple* tup = as<Tuple>(obj);

    for(native_int i = 0; i < tup->num_fields(); i++) {
      tmp = mark.call(tup->field[i]);
      if(tmp) mark.set(obj, &tup->field[i], tmp);
    }
  }

  void Tuple::Info::show(STATE, Object* self, int level) {
    Tuple* tup = as<Tuple>(self);
    native_int size = tup->num_fields();
    native_int stop = size < 6 ? size : 6;

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    ++level;
    for(native_int i = 0; i < stop; i++) {
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
    native_int size = tup->num_fields();
    native_int stop = size < 6 ? size : 6;

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    ++level;
    for(native_int i = 0; i < stop; i++) {
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
