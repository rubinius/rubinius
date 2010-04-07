#ifndef RBX_CAPI_HPP
#define RBX_CAPI_HPP

#include <sstream>
#include <string>

#include <cstdarg>
#include <cstdlib>
#include <cstring>

#include "builtin/nativemethod.hpp"

#include "object_utils.hpp"

#include "capi/tag.hpp"

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
    std::string& capi_get_constant_name(int type);

    /** Raise a RuntimeError error */
    void capi_raise_runtime_error(const char* reason);

    /** Raise a TypeError (convenience method). */
    void capi_raise_type_error(object_type type, Object* object);

    /** Raise backend */
    void capi_raise_backend(Exception* exception);

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
