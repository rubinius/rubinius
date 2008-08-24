#ifndef RBX_OOP_HPP
#define RBX_OOP_HPP

#include <stdint.h>
#include <sys/types.h>
#include <sstream>
#include <cstring>
#include <cassert>
#include <vector>

#include "prelude.hpp"
#include "builtin/object.hpp"
#include "exception.hpp"

/* Defines all the most common operations on for dealing with
 * objects, such as type checking and casting. */

namespace rubinius {

  void inspect(STATE, OBJECT);
  void inspect(STATE, SYMBOL);

  /* Given builtin-class +T+, return true if +obj+ is of class +T+ */
  template <class T>
    static inline bool kind_of(OBJECT obj) {
      if(obj->reference_p()) {
        return obj->obj_type == T::type;
      }
      return false;
    }

  /* Another version of kind_of that shouldn't be specialized for subtype
   * compatibility. */
  template <class T>
    static inline bool instance_of(OBJECT obj) {
      if(obj->reference_p()) {
        return obj->obj_type == T::type;
      }
      return false;
    }

  template <>
    static inline bool kind_of<Object>(OBJECT obj) {
      return true;
    }

  template <>
    static inline bool kind_of<Class>(OBJECT obj) {
      return obj->obj_type == ClassType || 
        obj->obj_type == MetaclassType ||
        obj->obj_type == IncModType;
    }


  /* Used when casting between object types.
   *
   * Given builtin class +T+, return +obj+ cast as type +T*+. If
   * +obj+ is not of type +T+, throw's a TypeError exception.
   * */
  template <class T>
    static inline T* as(OBJECT obj) {
      /* The 'obj &&' gives us additional saftey, checking for
       * NULL objects. */
      if(!obj || !kind_of<T>(obj)) TypeError::raise(T::type, obj);
      return (T*)obj;
    }

  template <>
    static inline Object* as<Object>(OBJECT obj) { return obj; }

  /* Similar to as<>, but returns NULL if the type is invalid. ONLY
   * use this when doing a conditional cast. */
  template <class T>
    static inline T* try_as(OBJECT obj) {
      /* The 'obj &&' gives us additional saftey, checking for
       * NULL objects. */
      if(obj && kind_of<T>(obj)) return (T*)obj;
      return NULL;
    }

  void type_assert(OBJECT obj, object_type type, const char* reason);
#define sassert(cond) if(!(cond)) Assertion::raise(#cond)

  /*
   * A rubinius object can be followed by:
   * - a series of fields, possibly including an ivar
   * - a series of bytes (ByteArray)
   * - a fast_context pointer
   */

  /* HACK: refactor this to use the state_setup_type code path. */
  struct wraps_struct {
    void *ptr;
    void (*mark)(void*);
    void (*free)(void*);
  };

#define MARK_WRAPPED_STRUCT(obj) do { \
  struct wraps_struct *s = (struct wraps_struct *)BYTES_OF(obj); \
  if(s->mark != NULL) { s->mark(s->ptr); } \
} while (0)

#define FREE_WRAPPED_STRUCT(obj) do { \
  struct wraps_struct *s = (struct wraps_struct *)BYTES_OF(obj); \
  if(s->free != NULL) { s->free(s->ptr); } \
} while (0)

};

#endif
