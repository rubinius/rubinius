#ifndef RBX_CAPI_HPP
#define RBX_CAPI_HPP

#include <sstream>
#include <string>

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "builtin/native_method.hpp"

#include "object_utils.hpp"

#include "capi/capi_constants.h"
#include "capi/tag.hpp"

#define ENTER_CAPI(state) (state->vm()->shared.enter_capi(state, __FILE__, __LINE__))
#define LEAVE_CAPI(state) (state->vm()->shared.leave_capi(state))

namespace rubinius {
  class Integer;
  class Object;
  class Symbol;

  namespace capi {

    /** Set what ObjectMark* object should be used by C-API */
    void set_current_mark(ObjectMark* mark);
    ObjectMark* current_mark();

    Symbol* prefixed_by(STATE, const char* prefix, size_t len, ID name);
    Symbol* prefixed_by(STATE, const char prefix, ID name);

    /** Return constant name mapping to 'type' */
    std::string capi_get_constant_name(int type);

    /** Raise a RuntimeError error */
    NORETURN(void capi_raise_runtime_error(const char* reason));

    /** Raise a TypeError (convenience method). */
    NORETURN(void capi_raise_type_error(object_type type, Object* object));

    /** Raise backend */
    void capi_raise_backend(Exception* exception);

    void capi_raise_backend(VALUE exception);

    void capi_raise_backend(VALUE klass, const char* reason);

    void capi_raise_break(VALUE obj);

    /** Get an Array object for a handle ensuring that any RARRAY data has
     * been flushed. */
    Array* capi_get_array(NativeMethodEnvironment* env, VALUE ary_handle);

    /** Update the RARRAY cache if one exists for this handle. */
    void capi_update_array(NativeMethodEnvironment* env, VALUE ary_handle);

    /** Get a String object for a handle ensuring that any RSTRING data has
     * been flushed. */
    String* capi_get_string(NativeMethodEnvironment* env, VALUE str_handle);

    /** Update the RSTRING cache if one exists for this handle. */
    void capi_update_string(NativeMethodEnvironment* env, VALUE str_handle);

    /** Get a Float object for a handle ensuring that RFLOAT data has
     *  been flushed. */
    Float* capi_get_float(NativeMethodEnvironment* env, VALUE float_handle);

    /** Wrap a C function in a Proc */
    Proc* wrap_c_function(void* func, VALUE cb, int arity);

    /** Call a ruby method and ignore cached handles, etc */
    VALUE capi_fast_call(VALUE receiver, ID method_name, int arg_count, ...);

    /** Converts a native type (int, uint, long) to a suitable Integer. */
    template<typename NativeType>
      VALUE capi_native2num(NativeType number) {
        NativeMethodEnvironment* env = NativeMethodEnvironment::get();
        return env->get_handle(Integer::from(env->state(), number));
      }

    /**
     *  Cast Object* into builtin T*.
     *
     *  Raises a TypeError if +obj+ is not of type +T*+. Calls rb_raise
     *  which uses setjmp/longjmp because C++ exceptions cannot be used
     *  in C functions (C does not know how to unwind the stack).
     *
     *  @see  vm/builtin/object.hpp
     */
    template <class T>
      static inline T* c_as(Object* obj) {
        if(!kind_of<T>(obj)) {
          capi_raise_type_error(T::type, obj);
        }

        return static_cast<T*>(obj);
      }

    /**
     *  Cast const Object* into builtin const T*.
     *
     *  Raises a TypeError if +obj+ is not of type +T*+. Calls rb_raise
     *  which uses setjmp/longjmp because C++ exceptions cannot be used
     *  in C functions (C does not know how to unwind the stack).
     *
     *  @see  vm/builtin/object.hpp
     */
    template <class T>
      static inline const T* c_as(const Object* obj) {
        if(!kind_of<T>(obj)) {
          capi_raise_type_error(T::type, obj);
        }

        return static_cast<const T*>(obj);
      }

    /** Nothing to convert; essentially a noop. */
    template <>
      SPECIALIZATION_STORAGE Object* c_as<Object>(Object* obj) {
        return obj;
      }

    /** Nothing to convert; essentially a noop. */
    template <>
      SPECIALIZATION_STORAGE const Object* c_as<Object>(const Object* obj) {
        return obj;
      }
  }
}

#endif
