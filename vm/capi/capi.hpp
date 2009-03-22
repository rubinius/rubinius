#ifndef RBX_CAPI_HPP
#define RBX_CAPI_HPP

#include <sstream>
#include <string>

#include <cstdarg>
#include <cstdlib>
#include <cstring>

#include "builtin/nativemethod.hpp"

#include "object_utils.hpp"

/* This must be the last file included here and capi.hpp must
 * be the last file included in any of the capi/(*).cpp files
 * because ruby.h redefines Qfalse and friends for the values
 * that are visible to C extensions.
 */
#include "ruby.h"

using namespace rubinius;

namespace rubinius {
  class Integer;
  class Object;
  class Symbol;
}

namespace capi {

  Symbol* prefixed_by(std::string prefix, std::string name);
  Symbol* prefixed_by(std::string prefix, ID name);

  /** Raise a TypeError (convenience method). */
  void capi_raise_type_error(object_type type, Object* object);

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

#endif
