#ifndef RBX_VM_OBJECT_UTILS_HPP
#define RBX_VM_OBJECT_UTILS_HPP

#include "class/object.hpp"
#include "exception.hpp"

/**
 *  @file   object_utils.hpp
 *
 *  Defines all the most common operations for dealing with
 *  objects, such as type checking and casting.
 */

#define SPECIALIZATION_STORAGE

namespace rubinius {

  /**
   *  Ruby type system subtype check.
   *
   *  Given builtin-class +T+, return true if +obj+ is of class +T+
   */
  template <class T>
    static bool kind_of(const MemoryHeader* obj) {
      if(obj->reference_p()) {
        return obj->type_id() == T::type;
      }
      return false;
    }

  template <class T>
    static bool kind_of(const Object* obj) {
      if(obj->reference_p()) {
        return obj->type_id() == T::type;
      }
      return false;
    }

  /**
   * A specialized version for completeness.
   */
  template <>
    bool kind_of<Object>(const Object* obj) {
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
    static bool instance_of(const Object* obj) {
      if(obj->reference_p()) {
        return obj->type_id() == T::type;
      }
      return false;
    }

  /**
   * A specialized version for completeness.
   */
  template <>
    bool instance_of<Object>(const Object* obj) {
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
    static T* as(Object* obj) {
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
    static const T* as(const Object* obj) {
      if(!kind_of<T>(obj)) {
        TypeError::raise(T::type, obj);
      }

      return static_cast<const T*>(obj);
    }

  /**
   * A specialized version for completeness.
   */
  template <>
    Object* as<Object>(Object* obj) {
      return obj;
    }

  /**
   * A specialized version for completeness.
   */
  template <>
    const Object* as<Object>(const Object* obj) {
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
    static T* as_instance(Object* obj) {
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
    static const T* as_instance(const Object* obj) {
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
    static T* try_as(MemoryHeader* obj) {
      if(!kind_of<T>(obj)) {
        return nullptr;
      }

      return static_cast<T*>(obj);
    }

  template <class T>
    static T* try_as(Object* obj) {
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
    static const T* try_as(const MemoryHeader* obj) {
      if(!kind_of<T>(obj)) {
        return nullptr;
      }

      return static_cast<const T*>(obj);
    }

  template <class T>
    static const T* try_as(const Object* obj) {
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
    static T* try_as_instance(Object* obj) {
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
    static const T* try_as_instance(const Object* obj) {
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
    static T* nil() { return static_cast<T*>(cNil); }

  /**
   *  Converts one type into another without a type check. This is
   *  like reinterprete_cast<>(), but we use it so we can easily
   *  find where we're doing explicit force casts.
   */
  template <class T>
    static T* force_as(ObjectHeader* obj) {
      return reinterpret_cast<T*>(obj);
    }

  template <class T>
    static const T* force_as(const ObjectHeader* obj) {
      return reinterpret_cast<const T*>(obj);
    }

  void type_assert(STATE, Object* obj, object_type type, const char* reason);

#include "kind_of.hpp"

}

#endif
