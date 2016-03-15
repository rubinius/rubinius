#ifndef RBX_VM_OBJECT_UTILS_HPP
#define RBX_VM_OBJECT_UTILS_HPP

#include "builtin/object.hpp"
#include "exception.hpp"

/**
 *  @file   object_utils.hpp
 *
 *  Defines all the most common operations for dealing with
 *  objects, such as type checking and casting.
 */

// A stupid work around for g++ changing it's behavior on 4.3

#if __clang__ || __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)
  #define SPECIALIZATION_STORAGE
#else
  #define SPECIALIZATION_STORAGE static inline
#endif

namespace rubinius {

  /**
   *  Ruby type system subtype check.
   *
   *  Given builtin-class +T+, return true if +obj+ is of class +T+
   */
  template <class T>
    static inline bool kind_of(const Object* obj) {
      if(obj->reference_p()) {
        return obj->type_id() == T::type;
      }
      return false;
    }

  /**
   * A specialized version for completeness.
   */
  template <>
    SPECIALIZATION_STORAGE bool kind_of<Object>(const Object* obj) {
      return true;
    }

  /**
   *  Ruby type system class check.
   *
   *  Another version of kind_of that shouldn't be specialized for subtype
   *  compatibility. I.e., only whether object's class is this specific
   *  one.
   */
  template <class T>
    static inline bool instance_of(const Object* obj) {
      if(obj->reference_p()) {
        return obj->type_id() == T::type;
      }
      return false;
    }

  /**
   * A specialized version for completeness.
   */
  template <>
    SPECIALIZATION_STORAGE bool instance_of<Object>(const Object* obj) {
      return obj->reference_p() && (obj->type_id() == ObjectType);
    }

  /*
   *  There is NO reason why as() or try_as() should be getting
   *  NULL arguments. That means something has not been properly
   *  initialised (to cNil if nothing else.)
   */

  /**
   *  Cast Object* into builtin T*.
   *
   *  Given builtin class +T+, return +obj+ cast as type +T*+. If
   *  +obj+ is not of type +T+, throw's a TypeError exception.
   *
   *  @see  builtin/object.cpp has specialised versions.
   */
  template <class T>
    static inline T* as(Object* obj) {
      if(!kind_of<T>(obj)) {
        TypeError::raise(T::type, obj);
      }

      return static_cast<T*>(obj);
    }

  /**
   *  Cast const Object* into builtin const T*.
   *
   *  Given builtin class +T+, return +obj+ cast as type +T*+. If
   *  +obj+ is not of type +T+, throw's a TypeError exception.
   *
   *  @see  builtin/object.cpp has specialised versions.
   */
  template <class T>
    static inline const T* as(const Object* obj) {
      if(!kind_of<T>(obj)) {
        TypeError::raise(T::type, obj);
      }

      return static_cast<const T*>(obj);
    }

  /**
   * A specialized version for completeness.
   */
  template <>
    SPECIALIZATION_STORAGE Object* as<Object>(Object* obj) {
      return obj;
    }

  /**
   * A specialized version for completeness.
   */
  template <>
    SPECIALIZATION_STORAGE const Object* as<Object>(const Object* obj) {
      return obj;
    }

  /**
   *  Cast Object* into builtin T*.
   *
   *  Given builtin class +T+, return +obj+ cast as type +T*+. If
   *  +obj+ is not exactly of type +T+, throw's a TypeError exception.
   *
   *  @see  builtin/object.cpp has specialised versions.
   */
  template <class T>
    static inline T* as_instance(Object* obj) {
      if(!instance_of<T>(obj)) {
        TypeError::raise(T::type, obj);
      }

      return static_cast<T*>(obj);
    }

  /**
   *  Cast const Object* into builtin const T*.
   *
   *  Given builtin class +T+, return +obj+ cast as type +T*+. If
   *  +obj+ is not exactly of type +T+, throw's a TypeError exception.
   *
   *  @see  builtin/object.cpp has specialised versions.
   */
  template <class T>
    static inline const T* as_instance(const Object* obj) {
      if(!instance_of<T>(obj)) {
        TypeError::raise(T::type, obj);
      }

      return static_cast<const T*>(obj);
    }

  /**
   *  Non-raising version of as().
   *
   *  Similar to as<>, but returns NULL if the type is invalid. ONLY
   *  use this when doing a conditional cast.
   *
   *  @see  builtin/object.cpp has specialised versions.
   */
  template <class T>
    static inline T* try_as(Object* obj) {
      if(!kind_of<T>(obj)) {
        return NULL;
      }

      return static_cast<T*>(obj);
    }

  /**
   *  Non-raising version of as(), for const objects.
   *
   *  Similar to as<>, but returns NULL if the type is invalid. ONLY
   *  use this when doing a conditional cast.
   *
   *  @see  builtin/object.cpp has specialised versions.
   */
  template <class T>
    static inline const T* try_as(const Object* obj) {
      if(!kind_of<T>(obj)) {
        return NULL;
      }

      return static_cast<const T*>(obj);
    }

  /**
   *  Non-raising version of as_instance().
   *
   *  Similar to as_instance<>, but returns NULL if the type is invalid. ONLY
   *  use this when doing a conditional cast.
   *
   *  @see  builtin/object.cpp has specialised versions.
   */
  template <class T>
    static inline T* try_as_instance(Object* obj) {
      if(!instance_of<T>(obj)) {
        return NULL;
      }

      return static_cast<T*>(obj);
    }

  /**
   *  Non-raising version of as(), for const objects.
   *
   *  Similar to as<>, but returns NULL if the type is invalid. ONLY
   *  use this when doing a conditional cast.
   *
   *  @see  builtin/object.cpp has specialised versions.
   */
  template <class T>
    static inline const T* try_as_instance(const Object* obj) {
      if(!instance_of<T>(obj)) {
        return NULL;
      }

      return static_cast<const T*>(obj);
    }

  /**
   *  Returns cNil cast to another type of rubinius::Object. Useful in
   *  cases where e.g. a String* would be returned but cNil is returned
   *  as an exceptional value.
   *
   */
  template <class T>
    static inline T* nil() { return static_cast<T*>(cNil); }

  /**
   *  Converts one type into another without a type check. This is
   *  like reinterprete_cast<>(), but we use it so we can easily
   *  find where we're doing explicit force casts.
   */
  template <class T>
    static inline T* force_as(ObjectHeader* obj) {
      return reinterpret_cast<T*>(obj);
    }

  template <class T>
    static inline const T* force_as(const ObjectHeader* obj) {
      return reinterpret_cast<const T*>(obj);
    }

  void type_assert(STATE, Object* obj, object_type type, const char* reason);

#include "gen/kind_of.hpp"

}

#endif
