#include "class/array.hpp"
#include "class/fixnum.hpp"
#include "class/object.hpp"
#include "class/proc.hpp"
#include "class/thread.hpp"
#include "class/lookup_table.hpp"
#include "memory.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "exception_point.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

#include <stdarg.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {

    Array* capi_get_array(NativeMethodEnvironment* env, VALUE val) {
      Handle* handle = Handle::from(val);
      Array* array = c_as<Array>(handle->object());
      handle->flush(env);

      return array;
    }

    void capi_update_array(NativeMethodEnvironment* env, VALUE array) {
      Handle::from(array)->update(env);
    }

    /* We were in C-land and now we are returning to Ruby-land. Since the C
     * program can freely assign to RArray.len and RArray.ptr, we account
     * for that when updating the Ruby Array with the C structure contents.
     *
     * Note that we must copy the total elements in the cached C array
     * regardless of the value of the len parameter because the C array
     * contents can be changed indepedently from the len parameter.
     *
     * See Handle::as_rarray below.
     */
    void flush_cached_rarray(NativeMethodEnvironment* env, Handle* handle) {
      if(handle->is_rarray()) {
        Array* array = c_as<Array>(handle->object());
        Tuple* tuple = array->tuple();
        RArray* rarray = handle->as_rarray(env);

        native_int size = tuple->num_fields();
        native_int num = 0;

        if(rarray->ptr != rarray->dmwmb) {
          // This is a very bad C extension. Assume len is valid.
          num = rarray->len;
        } else {
          num = rarray->aux.capa;
        }

        if(num > size) {
          tuple = Tuple::create(env->state(), rarray->aux.capa);
          array->tuple(env->state(), tuple);
        }

        array->start(env->state(), Fixnum::from(0));
        array->total(env->state(), Fixnum::from(rarray->len));

        for(native_int i = 0; i < num; i++) {
          tuple->put(env->state(), i, env->get_object(rarray->ptr[i]));
        }
      }
    }

    /* We were in Ruby-land and we are heading to C-land. In Ruby-land, we
     * may have updated the existing Array elements, appended new elements,
     * or shifted off elements. We account for this when updating the C
     * structure contents.
     *
     * We are potentially writing into a C structure that exists and that
     * may have been changed in C-land. It is possible for C code to change
     * both the len and ptr values of an RArray. We DO NOT EVER encourage
     * doing this, but we must account for it. The C code may also merely
     * change the contents of the array pointed to by ptr. Updating that
     * array with the current elements in the Ruby Array is the purpose of
     * this code.
     */
    void update_cached_rarray(NativeMethodEnvironment* env, Handle* handle) {
      if(handle->is_rarray()) {
        Array* array = c_as<Array>(handle->object());
        Tuple* tuple = array->tuple();
        RArray* rarray = handle->as_rarray(env);

        native_int size = tuple->num_fields();
        native_int start = array->start()->to_native();
        native_int num = 0;

        if(rarray->ptr != rarray->dmwmb) {
          // This is a very bad C extension. Assume len is valid
          // and do not change its value.
          num = rarray->len;
        } else {
          env->shared().capi_ds_lock().lock();

          if(rarray->aux.capa < size) {
            delete[] rarray->dmwmb;
            rarray->dmwmb = rarray->ptr = new VALUE[size];
            rarray->aux.capa = size;
          }
          num = rarray->aux.capa;
          rarray->len = array->size();

          env->shared().capi_ds_lock().unlock();
        }

        for(native_int i = 0, j = start; i < num && j < size; i++, j++) {
          rarray->ptr[i] = env->get_handle(tuple->at(j));
        }
      }
    }

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
    RArray* Handle::as_rarray(NativeMethodEnvironment* env) {
      if(type_ != cRArray) {
        env->shared().capi_ds_lock().lock();

        // Gotta double check since we're now lock and it might
        // have changed since we asked for the lock.
        if(type_ != cRArray) {
          Array* array = c_as<Array>(object());
          native_int size = array->tuple()->num_fields();

          RArray* ary = new RArray;
          VALUE* ptr = new VALUE[size];
          for(native_int i = 0; i < size; i++) {
            ptr[i] = env->get_handle(array->get(env->state(), i));
          }

          ary->dmwmb = ary->ptr = ptr;
          ary->len = array->size();
          ary->aux.capa = size;
          ary->aux.shared = Qfalse;

          type_ = cRArray;
          as_.rarray = ary;

          flush_ = flush_cached_rarray;
          update_ = update_cached_rarray;

          env->state()->memory()->make_capi_handle_cached(env->state(), this);
        }

        env->shared().capi_ds_lock().unlock();
      }

      return as_.rarray;
    }
  }
}

extern "C" {
  struct RArray* capi_rarray_struct(VALUE val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Handle* h = Handle::from(val);

    env->check_tracked_handle(h);
    return h->as_rarray(env);
  }

  VALUE rb_Array(VALUE object) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* obj = env->get_object(object);

    if(kind_of<Array>(obj)) {
      return object;
    }

    ID to_a_id = rb_intern("to_a");
    if(rb_respond_to(object, to_a_id)) {
      return rb_funcall(object, to_a_id, 0);
    }

    Array* array = Array::create(env->state(), 1);
    array->set(env->state(), 0, obj);

    return env->get_handle(array);
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

    Array* array = capi_get_array(env, self);
    return env->get_handle(array->aref(env->state(), Fixnum::from(index)));
  }

  VALUE rb_ary_each(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = capi_get_array(env, self);
    for(native_int i = 0; i < array->size(); i++) {
      rb_yield(env->get_handle(array->get(env->state(), i)));
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
    return env->get_handle(array);
  }

  VALUE rb_ary_new2(unsigned long length) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = Array::create(env->state(), length);

    return env->get_handle(array);
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
      Object* object = env->get_object(va_arg(args, VALUE));
      array->set(env->state(), i, object);
    }

    va_end(args);

    return env->get_handle(array);
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
        Object* object = env->get_object(objects[i]);
        array->set(env->state(), i, object);
      }
    }

    return env->get_handle(array);
  }

  VALUE rb_ary_pop(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = capi_get_array(env, self);
    Object* obj = array->pop(env->state());
    capi_update_array(env, self);

    return env->get_handle(obj);
  }

  VALUE rb_ary_push(VALUE self, VALUE object) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = capi_get_array(env, self);
    array->append(env->state(), env->get_object(object));
    capi_update_array(env, self);

    return self;
  }

  VALUE rb_ary_reverse(VALUE self) {
    return rb_funcall2(self, rb_intern("reverse!"), 0, NULL);
  }

  VALUE rb_ary_shift(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = capi_get_array(env, self);
    Object* obj = array->shift(env->state());
    capi_update_array(env, self);

    return env->get_handle(obj);
  }

  long rb_ary_size(VALUE self) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return capi_get_array(env, self)->size();
  }

  void rb_ary_store(VALUE self, long int index, VALUE object) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = capi_get_array(env, self);
    native_int total = array->size();

    if(index < 0) {
      index += total;
    }

    if(index < 0) {
      rb_raise(rb_eIndexError, "Index %li out of range", index - total);
    }

    array->set(env->state(), index, env->get_object(object));
    capi_update_array(env, self);
  }

  VALUE rb_ary_concat(VALUE self, VALUE second) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = capi_get_array(env, self);
    Array* other = capi_get_array(env, second);
    array->concat(env->state(), other);
    capi_update_array(env, self);
    return self;
  }

  VALUE rb_ary_plus(VALUE self, VALUE second) {
    return rb_funcall(self, rb_intern("+"), 1, second);
  }

  VALUE rb_ary_unshift(VALUE self, VALUE object) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = capi_get_array(env, self);
    array->unshift(env->state(), env->get_object(object));
    capi_update_array(env, self);

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
    if(ifunc == rb_each && kind_of<Array>(env->get_object(ary))) {
      for(long i = 0; i < rb_ary_size(ary); i++) {
        (*cb)(rb_ary_entry(ary, i), cb_data, Qnil);
      }

      return ary;
    }

    Proc* prc = capi::wrap_c_function((void*)cb, cb_data, ITERATE_BLOCK);

    env->set_outgoing_block(env->get_handle(prc));

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

    Object* obj = env->get_object(h_obj);

    Object* id = obj->id(state);

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
    obj = env->get_object(h_obj);
    id = obj->id(state);

    rectbl->remove(state, id);

    if(unwinding) env->current_ep()->return_to(env);

    return ret;
  }

  VALUE rb_inspecting_p(VALUE h_obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    STATE = env->state();

    Thread* thr = Thread::current(state);
    LookupTable* rectbl = thr->recursive_objects();

    Object* obj = env->get_object(h_obj);

    Object* id = obj->id(state);

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

    Object* obj = env->get_object(object);

    if(kind_of<Array>(obj)) {
      return object;
    }

    ID to_ary_id = rb_intern("to_ary");

    if(rb_respond_to(object, to_ary_id)) {
      return rb_funcall(object, to_ary_id, 0);
    } else {
      Array* array = Array::create(env->state(), 1);
      array->set(env->state(), 0, env->get_object(object));

      return env->get_handle(array);
    }
  }

  VALUE rb_ary_subseq(VALUE ary, long beg, long len) {
    if (beg > RARRAY_LEN(ary)) return Qnil;
    if (beg < 0 || len < 0) return Qnil;

    if (RARRAY_LEN(ary) < len || RARRAY_LEN(ary) < beg + len) {
      len = RARRAY_LEN(ary) - beg;
    }

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = c_as<Array>(env->get_object(ary));
    return env->get_handle(array->new_range(env->state(), Fixnum::from(beg), Fixnum::from(len)));
  }
}
