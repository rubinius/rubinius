#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/object.hpp"

#include "message.hpp"

#include "capi/capi.hpp"

using namespace capi;

extern "C" {
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

  VALUE rb_ary_dup(VALUE self_handle) {
    return rb_funcall2(self_handle, rb_intern("dup"), 0, NULL);
  }

  /* @todo Check 64-bit? */
  VALUE rb_ary_entry(VALUE self_handle, int index) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = c_as<Array>(env->get_object(self_handle));
    return env->get_handle(self->get(env->state(), index));
  }

  VALUE rb_ary_join(VALUE self_handle, VALUE separator_handle) {
    return rb_funcall(self_handle, rb_intern("join"), 1, separator_handle);
  }

  /** By default, Arrays have space for 16 elements. */
  static const unsigned long cCApiArrayDefaultCapacity = 16;

  VALUE rb_ary_new() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = Array::create(env->state(), cCApiArrayDefaultCapacity);
    return env->get_handle(array);
  }

  /* Shares implementation with rb_ary_new4! Change both if needed. */
  VALUE rb_ary_new2(unsigned long length) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = Array::create(env->state(), (length * 2));
    array->start(env->state(), Fixnum::from(0));
    array->total(env->state(), Fixnum::from(length));
    /* OK, so we are probably screwed anyway if a Fixnum is too small. :) */

    return env->get_handle(array);
  }

  /* Shares implementation with rb_ary_new2! Change both if needed. */
  VALUE rb_ary_new4(unsigned long length, const VALUE* object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* array = Array::create(env->state(), (length * 2));
    array->start(env->state(), Fixnum::from(0));
    array->total(env->state(), Fixnum::from(length));

    if (object_handle) {
      Object* object = env->get_object(*object_handle);

      for(std::size_t i = 0; i < length; ++i) {
        array->set(env->state(), i, object);
      }
    }

    return env->get_handle(array);
  }

  VALUE rb_ary_pop(VALUE self_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = c_as<Array>(env->get_object(self_handle));
    return env->get_handle(self->pop(env->state()));
  }

  VALUE rb_ary_push(VALUE self_handle, VALUE object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = c_as<Array>(env->get_object(self_handle));
    self->append(env->state(), env->get_object(object_handle));

    return self_handle;
  }

  VALUE rb_ary_reverse(VALUE self_handle) {
    return rb_funcall2(self_handle, rb_intern("reverse"), 0, NULL);
  }

  VALUE rb_ary_shift(VALUE self_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = c_as<Array>(env->get_object(self_handle));
    return env->get_handle(self->shift(env->state()));
  }

  size_t rb_ary_size(VALUE self_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = c_as<Array>(env->get_object(self_handle));

    return self->size();
  }

  void rb_ary_store(VALUE self_handle, long int index, VALUE object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = c_as<Array>(env->get_object(self_handle));
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
  }

  VALUE rb_ary_unshift(VALUE self_handle, VALUE object_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Array* self = c_as<Array>(env->get_object(self_handle));
    self->unshift(env->state(), env->get_object(object_handle));

    return self_handle;
  }

  VALUE rb_attr_get(VALUE obj_handle, ID attr_name) {
    return rb_ivar_get(obj_handle, attr_name);
  }
}
