#include "alloc.hpp"
#include "memory.hpp"
#include "object_utils.hpp"

#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/tuple.hpp"

#include <stdarg.h>
#include <sstream>

namespace rubinius {

  void Tuple::bootstrap(STATE) {
    GO(tuple).set(Class::bootstrap_class(state, G(object), TupleType));
    GO(rtuple).set(Class::bootstrap_class(state, G(tuple), RTupleType));
  }

  Tuple* Tuple::bounds_exceeded_error(STATE, const char* method, int index) {
    std::ostringstream msg;

    msg << method;
    msg << ": index " << index << " out of bounds for size " << num_fields();

    Exception::raise_object_bounds_exceeded_error(state, msg.str().c_str());
    return 0;
  }

  /* The Tuple#at primitive. */
  Object* Tuple::at_prim(STATE, Fixnum* index_obj) {
    intptr_t index = index_obj->to_native();

    if(index < 0 || num_fields() <= index) {
      return bounds_exceeded_error(state, "Tuple::at_prim", index);
    }

    return field[index];
  }

  /* The Tuple#put primitive. */
  Object* Tuple::put_prim(STATE, Fixnum* index, Object* val) {
    intptr_t idx = index->to_native();

    if(idx < 0 || num_fields() <= idx) {
      return bounds_exceeded_error(state, "Tuple::put_prim", idx);
    }

    field[idx] = val;

    write_barrier(state, val);

    return val;
  }

  /* The Tuple#fields primitive. */
  Object* Tuple::fields_prim(STATE) {
    return Integer::from(state, num_fields());
  }

  Tuple* Tuple::create(STATE, intptr_t fields) {
    if(fields < 0) {
      Exception::raise_argument_error(state, "negative tuple size");
    }

    Tuple* tup = state->memory()->new_fields<Tuple>(state, G(tuple), fields);
    Tuple::initialize(state, tup);

    return tup;
  }

  Tuple* Tuple::allocate(STATE, Object* self, Fixnum* fields) {
    intptr_t size = fields->to_native();

    if(size < 0) {
      Exception::raise_argument_error(state, "negative tuple size");
    }

    Tuple* tuple = create(state, fields->to_native());
    tuple->klass(state, as<Class>(self));
    return tuple;
  }

  Tuple* Tuple::from(STATE, intptr_t fields, ...) {
    Tuple* tup = Tuple::create(state, fields);
    va_list ar;

    va_start(ar, fields);
    for(intptr_t i = 0; i < fields; i++) {
      Object *obj = va_arg(ar, Object*);
      // fields equals size so bounds checking is unecessary
      tup->put(state, i, obj);
    }
    va_end(ar);

    return tup;
  }

  Tuple* Tuple::copy_from(STATE, Tuple* other, Fixnum* start, Fixnum *length, Fixnum* dest) {
    intptr_t osize = other->num_fields();
    intptr_t size = this->num_fields();

    intptr_t src_start = start->to_native();
    intptr_t dst_start = dest->to_native();
    intptr_t len = length->to_native();

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
        for(intptr_t dest_idx = dst_start + len - 1,
                       src_idx  = src_start + len - 1;
            src_idx >= src_start;
            src_idx--, dest_idx--) {
          this->field[dest_idx] = this->field[src_idx];
        }
      } else {
        // left shift
        for(intptr_t dest_idx = dst_start,
                       src_idx  = src_start;
            src_idx < src_start + len;
            src_idx++, dest_idx++) {
          this->field[dest_idx] = this->field[src_idx];
        }
      }

    } else {
      for(intptr_t src = src_start, dst = dst_start;
          src < (src_start + len);
          ++src, ++dst) {
        // Since we have carefully checked the bounds we don't need
        // to do it in at/put
        Object *obj = other->field[src];
        // but this is necessary to keep the GC happy
        field[dst] = obj;
        write_barrier(state, obj);
      }
    }

    return this;
  }

  intptr_t Tuple::delete_inplace(intptr_t lend, intptr_t len, Object* obj) {
    intptr_t rend = lend + len;
    intptr_t i = lend;
    while(i < rend) {
      if(at(i)->equal_p(obj)) {
        intptr_t j = i;
        ++i;
        while(i < rend) {
          Object *val = field[i];
          if(!val->equal_p(obj)) {
            // no need to set write_barrier since it's already
            // referenced to this object
            field[j] = val;
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
        return rend-j;
      }
      ++i;
    }
    return 0;
  }

  Fixnum* Tuple::delete_inplace_prim(STATE, Fixnum *start, Fixnum *length, Object *obj) {
    intptr_t size = this->num_fields();
    intptr_t len  = length->to_native();
    intptr_t lend = start->to_native();
    intptr_t rend = lend + len;

    if(size == 0 || len == 0) return Fixnum::from(0);
    if(lend < 0 || lend >= size) {
      return force_as<Fixnum>(bounds_exceeded_error(state, "Tuple::delete_inplace", lend));
    }

    if(rend < 0 || rend > size) {
      return force_as<Fixnum>(bounds_exceeded_error(state, "Tuple::delete_inplace", rend));
    }

    return Fixnum::from(delete_inplace(lend, len, obj));
  }

  /** @todo Add some error checking/handling and
   *  evaluate corner cases, and add tests... --rue
   */
  Tuple* Tuple::lshift_inplace(STATE, Fixnum* shift) {
    intptr_t size = this->num_fields();
    const intptr_t start = shift->to_native();

    assert(start >= 0);

    if(start > 0) {
      intptr_t i = 0;
      intptr_t j = start;

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
    intptr_t start = o_start->to_native();
    intptr_t total = o_total->to_native();

    if(total <= 0 || start < 0 || start >= num_fields()) return this;

    intptr_t end = start + total - 1;
    if(end >= num_fields()) end = num_fields() - 1;

    Object** pos1 = field + start;
    Object** pos2 = field + end;

    Object* tmp;
    while(pos1 < pos2) {
      tmp = *pos1;
      *pos1++ = *pos2;
      *pos2-- = tmp;
    }

    return this;
  }

  // @todo performance primitive; could be replaced with Ruby
  Tuple* Tuple::pattern(STATE, Fixnum* size, Object* val) {
    intptr_t cnt = size->to_native();

    if(cnt < 0) {
      Exception::raise_argument_error(state, "negative tuple size");
    }

    Tuple* tuple = state->memory()->new_fields<Tuple>(state, G(tuple), cnt);

    for(intptr_t i = 0; i < cnt; i++) {
      tuple->field[i] = val;
    }

    tuple->write_barrier(state, val);

    return tuple;
  }

  Tuple* Tuple::tuple_dup(STATE) {
    intptr_t fields = num_fields();

    Tuple* tup = Tuple::create(state, fields);

    for(intptr_t i = 0; i < fields; i++) {
      tup->put(state, i, field[i]);
    }

    return tup;
  }

  size_t Tuple::Info::object_size(const ObjectHeader* obj) {
    return force_as<Tuple>(obj)->full_size();
  }

  void Tuple::Info::mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    Tuple* tup = as<Tuple>(obj);

    for(intptr_t i = 0; i < tup->num_fields(); i++) {
      f(state, &tup->field[i]);
    }
  }

  void Tuple::Info::before_visit(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    Tuple* tup = as<Tuple>(obj);

    for(intptr_t i = 0; i < tup->num_fields(); i++) {
      f(state, &tup->field[i]);
    }
  }

  void Tuple::Info::show(STATE, Object* self, int level) {
    Tuple* tup = as<Tuple>(self);
    intptr_t size = tup->num_fields();
    intptr_t stop = size < 6 ? size : 6;

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    ++level;
    for(intptr_t i = 0; i < stop; i++) {
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
    intptr_t size = tup->num_fields();
    intptr_t stop = size < 6 ? size : 6;

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    ++level;
    for(intptr_t i = 0; i < stop; i++) {
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

  void RTuple::initialize(STATE, RTuple* obj) {
    for(intptr_t i = 0; i < obj->num_fields(); i++) {
      reinterpret_cast<VALUE*>(obj->field)[i] = MemoryHandle::value(cNil);
    }
  }

  RTuple* RTuple::from(STATE, Tuple* tuple) {
    RTuple* rtuple = create(state, tuple->num_fields());

    for(intptr_t i = 0; i < tuple->num_fields(); i++) {
      reinterpret_cast<VALUE*>(rtuple->field)[i] = MemoryHandle::value(tuple->at(i));
    }

    return rtuple;
  }

  RTuple* RTuple::create(STATE, intptr_t fields) {
    if(fields < 0) {
      Exception::raise_argument_error(state, "negative RTuple size");
    }

    RTuple* rtup = state->memory()->new_fields_pinned<RTuple>(state, G(rtuple), fields);
    RTuple::initialize(state, rtup);

    return rtup;
  }

  RTuple* RTuple::allocate(STATE, Object* self, Fixnum* fields) {
    intptr_t size = fields->to_native();

    if(size < 0) {
      Exception::raise_argument_error(state, "negative RTuple size");
    }

    RTuple* tuple = create(state, fields->to_native());
    tuple->klass(state, as<Class>(self));

    return tuple;
  }

  Object* RTuple::at_prim(STATE, Fixnum* idx) {
    intptr_t index = idx->to_native();

    if(index < 0 || num_fields() <= index) {
      return bounds_exceeded_error(state, "RTuple::at_prim", index);
    }

    return MemoryHandle::object(reinterpret_cast<VALUE>(field[index]));
  }

  Object* RTuple::put_prim(STATE, Fixnum* idx, Object* val) {
    intptr_t index = idx->to_native();

    if(index < 0 || num_fields() <= index) {
      return bounds_exceeded_error(state, "RTuple::put_prim", index);
    }

    reinterpret_cast<VALUE*>(field)[index] = MemoryHandle::value(val);

    write_barrier(state, val);

    return val;
  }

  void RTuple::Info::mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    RTuple* rtup = as<RTuple>(obj);

    VALUE* ptr = reinterpret_cast<VALUE*>(rtup->field);

    for(intptr_t i = 0; i < rtup->num_fields(); i++) {
      Object* slot = MemoryHandle::object(ptr[i]);
      f(state, &slot);
      ptr[i] = MemoryHandle::value(slot);
    }
  }

  void RTuple::Info::before_visit(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    RTuple* rtup = as<RTuple>(obj);

    VALUE* ptr = reinterpret_cast<VALUE*>(rtup->field);

    for(intptr_t i = 0; i < rtup->num_fields(); i++) {
      Object* slot = MemoryHandle::object(ptr[i]);
      f(state, &slot);
      ptr[i] = MemoryHandle::value(slot);
    }
  }
}
