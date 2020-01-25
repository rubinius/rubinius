#include "class/array.hpp"
#include "class/fixnum.hpp"
#include "class/lookup_table.hpp"
#include "class/object.hpp"
#include "class/proc.hpp"
#include "class/thread.hpp"
#include "class/tuple.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "exception_point.hpp"
#include "memory.hpp"
#include "object_utils.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

#include <stdarg.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  /* It is possible to create an Array in MRI whose capacity exceeds its
   * current length (eg rb_ary_new2(long length)). The same is true in
   * Rubinius where Array has a size() attribute and is backed by an
   * instance of Tuple, whose size will always be gte Array::size().  The
   * RArray structure in MRI accounts for this and we must as well when
   * updating and syncing the cached values. For example:
   *
   *   for(i = 0; i < len; i++) {
   *     RARRAY(ary)->ptr[i] = obj
   *   }
   *   RARRAY(ary)->len = len
   */

  RArray* MemoryHandle::get_rarray(STATE) {
    if(rarray_p()) {
      return reinterpret_cast<RArray*>(data());
    } else if(object_type_p()) {
      Array* array = c_as<Array>(object());;
      array->set_type_specific(state, Array::eRArray);

      RTuple* rtuple = RTuple::from(state, array->tuple());
      array->tuple(state, rtuple);

      intptr_t size = array->size();
      VALUE* ptr = reinterpret_cast<VALUE*>(rtuple->field + array->offset());

      RArray* rarray = new RArray;

      rarray->dmwmb = rarray->ptr = ptr;
      rarray->len = size;
      rarray->aux.capa = size;
      rarray->aux.shared = Qfalse;

      set_rarray(rarray);
      return rarray;
    } else {
      Exception::raise_runtime_error(state, "C-API handle invalid reference as RArray");
    }
  }

  void MemoryHandle::read_rarray(STATE) {
    if(rarray_p()) {
      Array* array = as<Array>(object());
      RArray* rarray = reinterpret_cast<RArray*>(data());

      if(array->size() != rarray->len) {
        if(rarray->len <= array->tuple()->num_fields() - array->offset()) {
          array->total(Fixnum::from(rarray->len));
        } else {
          Exception::raise_runtime_error(state, "RArray length attribute is greater than capacity");
        }
      }
    }
  }

  void MemoryHandle::write_rarray(STATE) {
    if(rarray_p()) {
      Array* array = as<Array>(object());
      RArray* rarray = reinterpret_cast<RArray*>(data());

      RTuple* rtuple;

      if(array->tuple()->type_id() == RTupleType) {
        rtuple = reinterpret_cast<RTuple*>(array->tuple());
      } else {
        rtuple = RTuple::from(state, array->tuple());
        array->tuple(state, rtuple);
      }

      intptr_t size = array->size();
      VALUE* ptr = reinterpret_cast<VALUE*>(rtuple->field + array->offset());

      rarray->dmwmb = rarray->ptr = ptr;
      rarray->len = array->size();
      rarray->aux.capa = size;
      rarray->aux.shared = Qfalse;
    }
  }
}

extern "C" {
  struct RArray* capi_rarray_struct(VALUE value) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return MemoryHandle::from(value)->get_rarray(env->state());
  }

  VALUE rb_Array(VALUE object) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* obj = MemoryHandle::object(object);

    if(kind_of<Array>(obj)) {
      return object;
    }

    ID to_a_id = rb_intern("to_a");
    if(rb_respond_to(object, to_a_id)) {
      return rb_funcall(object, to_a_id, 0);
    }

    Array* array = Array::create(env->state(), 1);
    array->set(env->state(), 0, obj);

    return MemoryHandle::value(array);
  }

  VALUE rb_ary_clear(VALUE self) {
    return rb_funcall2(self, rb_intern("clear"), 0, NULL);
  }

  VALUE rb_ary_delete(VALUE self, VALUE item) {
    return rb_funcall(self, rb_intern("delete"), 1, item);
  }

  VALUE rb_ary_delete_at(VALUE self, long idx) {
    return rb_funcall(self, rb_intern("delete_at"), 1, INT2FIX(idx));
  }

  VALUE rb_ary_dup(VALUE self) {
    return rb_funcall2(self, rb_intern("dup"), 0, NULL);
  }

  /* @todo Check 64-bit? */
  VALUE rb_ary_entry(VALUE self, long index) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = MemoryHandle::object<Array>(self);
    return MemoryHandle::value(array->aref(env->state(), Fixnum::from(index)));
  }

  VALUE rb_ary_each(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = MemoryHandle::object<Array>(self);

    for(intptr_t i = 0; i < array->size(); i++) {
      rb_yield(MemoryHandle::value(array->get(env->state(), i)));
    }

    return self;
  }

  VALUE rb_ary_includes(VALUE self, VALUE obj) {
    return rb_funcall(self, rb_intern("include?"), 1, obj);
  }

  VALUE rb_ary_join(VALUE self, VALUE separator) {
    return rb_funcall(self, rb_intern("join"), 1, separator);
  }

  VALUE rb_ary_to_s(VALUE self) {
    return rb_funcall(self, rb_intern("to_s"), 0);
  }

  /** By default, Arrays have space for 16 elements. */
  static const unsigned long cCApiArrayDefaultCapacity = 16;

  VALUE rb_ary_new() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = Array::create(env->state(), cCApiArrayDefaultCapacity);
    return MemoryHandle::value(array);
  }

  VALUE rb_ary_new2(unsigned long length) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = Array::create(env->state(), length);

    return MemoryHandle::value(array);
  }

  VALUE rb_ary_new3(unsigned long length, ...) {
    va_list args;
    va_start(args, length);

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = Array::create(env->state(), length);
    array->start(env->state(), Fixnum::from(0));
    array->total(env->state(), Fixnum::from(length));

    for(unsigned long i = 0; i < length; ++i) {
      // @todo determine if we need to check these objects for whether
      // they are arrays and flush any caches
      Object* object = MemoryHandle::object(va_arg(args, VALUE));
      array->set(env->state(), i, object);
    }

    va_end(args);

    return MemoryHandle::value(array);
  }

  VALUE rb_ary_new4(unsigned long length, const VALUE* objects) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = Array::create(env->state(), length);
    array->start(env->state(), Fixnum::from(0));
    array->total(env->state(), Fixnum::from(length));

    if(objects) {
      for(std::size_t i = 0; i < length; ++i) {
        // @todo determine if we need to check these objects for whether
        // they are arrays and flush any caches
        Object* object = MemoryHandle::object(objects[i]);
        array->set(env->state(), i, object);
      }
    }

    return MemoryHandle::value(array);
  }

  VALUE rb_ary_pop(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = MemoryHandle::object<Array>(self);
    Object* obj = array->pop(env->state());

    return MemoryHandle::value(obj);
  }

  VALUE rb_ary_push(VALUE self, VALUE object) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = MemoryHandle::object<Array>(self);
    array->append(env->state(), MemoryHandle::object(object));

    return self;
  }

  VALUE rb_ary_reverse(VALUE self) {
    return rb_funcall2(self, rb_intern("reverse!"), 0, NULL);
  }

  VALUE rb_ary_shift(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = MemoryHandle::object<Array>(self);
    Object* obj = array->shift(env->state());

    return MemoryHandle::value(obj);
  }

  long rb_ary_size(VALUE self) {
    return MemoryHandle::object<Array>(self)->size();
  }

  void rb_ary_store(VALUE self, long int index, VALUE object) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = MemoryHandle::object<Array>(self);
    intptr_t total = array->size();

    if(index < 0) {
      index += total;
    }

    if(index < 0) {
      rb_raise(rb_eIndexError, "Index %li out of range", index - total);
    }

    array->set(env->state(), index, MemoryHandle::object(object));
  }

  VALUE rb_ary_concat(VALUE self, VALUE second) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = MemoryHandle::object<Array>(self);
    Array* other = MemoryHandle::object<Array>(second);
    array->concat(env->state(), other);

    return self;
  }

  VALUE rb_ary_plus(VALUE self, VALUE second) {
    return rb_funcall(self, rb_intern("+"), 1, second);
  }

  VALUE rb_ary_unshift(VALUE self, VALUE object) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = MemoryHandle::object<Array>(self);
    array->unshift(env->state(), MemoryHandle::object(object));

    return self;
  }

  VALUE rb_assoc_new(VALUE first, VALUE second) {
    return rb_funcall(rb_cArray, rb_intern("[]"), 2, first, second);
  }

  VALUE rb_ary_aref(int argc, VALUE *argv, VALUE self) {
    return rb_funcall2(self, rb_intern("[]"), argc, argv);
  }

  VALUE rb_each(VALUE ary) {
    return rb_funcall(ary, rb_intern("each"), 0);
  }

  VALUE rb_iterate(VALUE(*ifunc)(VALUE), VALUE ary, VALUE(*cb)(ANYARGS),
                   VALUE cb_data)
  {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    // Minor optimization.
    if(ifunc == rb_each && kind_of<Array>(MemoryHandle::object(ary))) {
      for(long i = 0; i < rb_ary_size(ary); i++) {
        (*cb)(rb_ary_entry(ary, i), cb_data, Qnil);
      }

      return ary;
    }

    Proc* prc = capi::wrap_c_function((void*)cb, cb_data, ITERATE_BLOCK);

    env->set_outgoing_block(MemoryHandle::value(prc));

    return (*ifunc)(ary);
  }

  void rb_mem_clear(VALUE* ary, int len) {
    for(int i = 0; i < len; i++) {
      ary[i] = Qnil;
    }
  }

  VALUE rb_protect_inspect(VALUE (*func)(VALUE a, VALUE b), VALUE h_obj, VALUE h_arg) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    STATE = env->state();

    Thread* thr = Thread::current(state);
    LookupTable* rectbl = thr->recursive_objects();

    Object* obj = MemoryHandle::object(h_obj);

    Object* id = obj->object_id(state);

    bool found = false;
    rectbl->fetch(state, id, &found);

    if(found) {
      return (*func)(h_obj, h_arg);
    }

    rectbl->store(state, id, cTrue);

    VALUE ret = Qnil;

    ExceptionPoint ep(env);
    PLACE_EXCEPTION_POINT(ep);

    bool unwinding = false;

    if(unlikely(ep.jumped_to())) {
      unwinding = true;
    } else {
      ret = (*func)(h_obj, h_arg);
    }

    ep.pop(env);

    // Get the thread and table again, the GC might have fun.
    thr = Thread::current(state);
    rectbl = thr->recursive_objects();
    obj = MemoryHandle::object(h_obj);
    id = obj->object_id(state);

    rectbl->remove(state, id);

    if(unwinding) env->current_ep()->return_to(env);

    return ret;
  }

  VALUE rb_inspecting_p(VALUE h_obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    STATE = env->state();

    Thread* thr = Thread::current(state);
    LookupTable* rectbl = thr->recursive_objects();

    Object* obj = MemoryHandle::object(h_obj);

    Object* id = obj->object_id(state);

    bool found = false;
    rectbl->fetch(state, id, &found);

    if(found) return Qtrue;
    return Qfalse;
  }

  VALUE rb_ary_freeze(VALUE ary) {
    return rb_obj_freeze(ary);
  }

  VALUE rb_ary_to_ary(VALUE object) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* obj = MemoryHandle::object(object);

    if(kind_of<Array>(obj)) {
      return object;
    }

    ID to_ary_id = rb_intern("to_ary");

    if(rb_respond_to(object, to_ary_id)) {
      return rb_funcall(object, to_ary_id, 0);
    } else {
      Array* array = Array::create(env->state(), 1);
      array->set(env->state(), 0, MemoryHandle::object(object));

      return MemoryHandle::value(array);
    }
  }

  VALUE rb_ary_subseq(VALUE ary, long beg, long len) {
    if (beg > RARRAY_LEN(ary)) return Qnil;
    if (beg < 0 || len < 0) return Qnil;

    if (RARRAY_LEN(ary) < len || RARRAY_LEN(ary) < beg + len) {
      len = RARRAY_LEN(ary) - beg;
    }

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = MemoryHandle::object<Array>(ary);
    return MemoryHandle::value(array->new_range(env->state(), Fixnum::from(beg), Fixnum::from(len)));
  }
}
