#ifndef RBX_CAPI_HPP
#define RBX_CAPI_HPP

#include <sstream>
#include <string>

#include <cstdarg>
#include <cstdlib>
#include <cstring>

#include "builtin/nativemethod.hpp"

#include "object_utils.hpp"

namespace rubinius {
  class Integer;
  class Object;
  class Symbol;

  namespace capi {

    /* VALUE and ID are defined here rather than including ruby.h to allow
     * for sharing C-API function prototypes between the VM proper and the
     * C-API code. @see vm/capi/ruby.h for explanations of VALUE and ID.
     */
#define VALUE intptr_t
#define ID    intptr_t

    Symbol* prefixed_by(std::string prefix, std::string name);
    Symbol* prefixed_by(std::string prefix, ID name);

    /** Return constant name mapping to 'type' */
    std::string& capi_get_constant_name(int type);

    /** Raise a RuntimeError error */
    void capi_raise_runtime_error(const char* reason);

    /** Raise a TypeError (convenience method). */
    void capi_raise_type_error(object_type type, Object* object);

    /** Update contents of Array objects for which RARRAY was called. */
    void capi_rarray_flush();

    /** Update contents of String objects for which RSTRING was called. */
    void capi_rstring_flush();

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
