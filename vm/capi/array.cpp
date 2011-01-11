#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/object.hpp"
#include "builtin/proc.hpp"
#include "builtin/thread.hpp"
#include "builtin/lookuptable.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "exception_point.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

#include <stdarg.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {

    Array* capi_get_array(NativeMethodEnvironment* env, VALUE val) {
      if(!env) env = NativeMethodEnvironment::get();

      Handle* handle = Handle::from(val);
      Array* array = c_as<Array>(handle->object());
      handle->flush(env);

      return array;
    }

    void capi_update_array(NativeMethodEnvironment* env, VALUE ary_handle) {
      if(!env) env = NativeMethodEnvironment::get();

      Handle* handle = Handle::from(ary_handle);
      handle->update(env);
    }

    void flush_cached_rarray(NativeMethodEnvironment* env, Handle* handle) {
      if(handle->is_rarray()) {
        Array* array = as<Array>(handle->object());
        RArray* rarray = handle->as_rarray(env);

        size_t size = array->size();

        if(size != rarray->len) {
          Tuple* tuple = Tuple::create(env->state(), rarray->len);
          array->tuple(env->state(), tuple);
          array->start(env->state(), Fixnum::from(0));
          array->total(env->state(), Fixnum::from(rarray->len));
        }

        for(size_t i = 0; i < size; i++) {
          array->set(env->state(), i, env->get_object(rarray->ptr[i]));
        }
      }
    }

    void update_cached_rarray(NativeMethodEnvironment* env, Handle* handle) {
      if(handle->is_rarray()) {
        Array* array = c_as<Array>(handle->object());
        RArray* rarray = handle->as_rarray(env);

        size_t size = array->size();

        if(rarray->len != size) {
          delete[] rarray->dmwmb;
          rarray->dmwmb = rarray->ptr = new VALUE[size];
          rarray->aux.capa = rarray->len = size;
        }

        for(size_t i = 0; i < size; i++) {
          rarray->ptr[i] = env->get_handle(array->get(env->state(), i));
        }
      }
    }

    RArray* Handle::as_rarray(NativeMethodEnvironment* env) {
      if(type_ != cRArray) {
        Array* array = c_as<Array>(object());
        size_t size = array->size();

        RArray* ary = new RArray;
        VALUE* ptr = new VALUE[size];
        for(size_t i = 0; i < size; i++) {
          ptr[i] = env->get_handle(array->get(env->state(), i));
        }

        ary->dmwmb = ary->ptr = ptr;
        ary->aux.capa = ary->len = size;
        ary->aux.shared = Qfalse;

        type_ = cRArray;
        as_.rarray = ary;

        flush_ = flush_cached_rarray;
        update_ = update_cached_rarray;

        env->state()->shared.global_handles()->move(this,
            env->state()->shared.cached_handles());
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

  VALUE rb_Array(VALUE obj_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* obj = env->get_object(obj_handle);

    if (kind_of<Array>(obj)) {
      return obj_handle;
    }

    Array* array = Array::create(env->state(), 1);
    array->set(env->state(), 0, obj);

    return env->get_handle(array);
  }

  VALUE rb_ary_clear(VALUE self_handle) {
    return rb_funcall2(self_handle, rb_intern("clear"), 0, NULL);
  }

  VALUE rb_ary_delete(VALUE self_handle, VALUE item) {
    return rb_funcall(self_handle, rb_intern("delete"), 1, item);
  }

  VALUE rb_ary_delete_at(VALUE self_handle, long idx) {
    return rb_funcall(self_handle, rb_intern("delete_at"), 1, INT2FIX(idx));
  }

  VALUE rb_ary_dup(VALUE self_handle) {
    return rb_funcall2(self_handle, rb_intern("dup"), 0, NULL);
  }

  /* @todo Check 64-bit? */
  VALUE rb_ary_entry(VALUE self_handle, int index) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = capi_get_array(env, self_handle);
    return env->get_handle(self->aref(env->state(), Fixnum::from(index)));
  }

  VALUE rb_ary_each(VALUE self_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = capi_get_array(env, self_handle);
    for(size_t i = 0; i < self->size(); i++) {
      rb_yield(env->get_handle(self->get(env->state(), i)));
    }

    return self_handle;
  }

  VALUE rb_ary_includes(VALUE self, VALUE obj) {
    return rb_funcall(self, rb_intern("include?"), 1, obj);
  }

  VALUE rb_ary_join(VALUE self_handle, VALUE separator_handle) {
    return rb_funcall(self_handle, rb_intern("join"), 1, separator_handle);
  }

  VALUE rb_ary_to_s(VALUE self_handle) {
    return rb_funcall(self_handle, rb_intern("to_s"), 0);
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

  VALUE rb_ary_new4(unsigned long length, const VALUE* object_handles) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = Array::create(env->state(), length);
    array->start(env->state(), Fixnum::from(0));
    array->total(env->state(), Fixnum::from(length));

    if (object_handles) {
      for(std::size_t i = 0; i < length; ++i) {
        // @todo determine if we need to check these objects for whether
        // they are arrays and flush any caches
        Object* object = env->get_object(object_handles[i]);
        array->set(env->state(), i, object);
      }
    }

    return env->get_handle(array);
  }

  VALUE rb_ary_pop(VALUE self_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = capi_get_array(env, self_handle);
    Object* obj = self->pop(env->state());
    capi_update_array(env, self_handle);

    return env->get_handle(obj);
  }

  VALUE rb_ary_push(VALUE self_handle, VALUE object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = capi_get_array(env, self_handle);
    self->append(env->state(), env->get_object(object_handle));
    capi_update_array(env, self_handle);

    return self_handle;
  }

  VALUE rb_ary_reverse(VALUE self_handle) {
    return rb_funcall2(self_handle, rb_intern("reverse"), 0, NULL);
  }

  VALUE rb_ary_shift(VALUE self_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = capi_get_array(env, self_handle);
    Object* obj = self->shift(env->state());
    capi_update_array(env, self_handle);

    return env->get_handle(obj);
  }

  size_t rb_ary_size(VALUE self_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = capi_get_array(env, self_handle);

    return self->size();
  }

  void rb_ary_store(VALUE self_handle, long int index, VALUE object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = capi_get_array(env, self_handle);
    size_t total = self->size();

    if(index < 0) {
      index += total;
    }

    if(index < 0) {
      std::ostringstream error;
      error << "Index " << (index - total) << " out of range!";
      rb_raise(rb_eIndexError, error.str().c_str());
    }

    self->set(env->state(), index, env->get_object(object_handle));
    capi_update_array(env, self_handle);
  }

  VALUE rb_ary_unshift(VALUE self_handle, VALUE object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = capi_get_array(env, self_handle);
    self->unshift(env->state(), env->get_object(object_handle));
    capi_update_array(env, self_handle);

    return self_handle;
  }

  VALUE rb_assoc_new(VALUE first, VALUE second) {
    return rb_funcall(rb_cArray, rb_intern("[]"), 2, first, second);
  }

  VALUE rb_ary_aref(int argc, VALUE *argv, VALUE self_handle) {
    return rb_funcall2(self_handle, rb_intern("[]"), argc, argv);
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
      for(size_t i = 0; i < rb_ary_size(ary); i++) {
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

    rectbl->store(state, id, RBX_Qtrue);

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
}
