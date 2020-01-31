#ifndef RBX_DEFINES_HPP
#define RBX_DEFINES_HPP

/**
 *  @file   defines.hpp
 *
 *  Notably here, Symbols, Fixnums and true/false/nil are actually stored
 *  directly in the pointer value (and distinguished by the tag, see
 *  memory/header.hpp) but provided we do not attempt to dereference it, we
 *  can ALSO define a class and treat the pointer values as if they were real
 *  pointers to real objects of that class where typing is concerned. In the
 *  "instance methods", the this pointer is still the correct pointer value
 *  and can thus be used for the calculations needed.
 *
 *  @see   memory/header.hpp
 */

#include <stdint.h>

#include "util/optimize.hpp"

namespace rubinius {
  class Class;
  class Fixnum;
  class ObjectHeader;
  class ThreadState;
  class Symbol;

  typedef intptr_t hashval;

  #define STATE rubinius::ThreadState* state
  #define G(whatever) state->globals().whatever.get()
  #define GO(whatever) state->globals().whatever

  namespace memory {
    void write_barrier(STATE, ObjectHeader* object, Fixnum* value);
    void write_barrier(STATE, ObjectHeader* object, Symbol* value);
    void write_barrier(STATE, ObjectHeader* object, ObjectHeader* value);
    void write_barrier(STATE, ObjectHeader* object, Class* value);
  }

/**
 *  Create a writer method for a slot.
 *
 *  For attr_writer(foo, SomeClass), creates void foo(STATE, SomeClass* obj)
 *  that sets the instance variable foo_ to the object given and runs the write
 *  barrier.
 */
#define attr_writer(name, type) \
  private: \
    type* _ ## name ## _; \
  public: \
    void name(type* obj) { \
      _ ## name ## _ = obj; \
    } \
    template<typename T> \
    void name(T* state, type* obj) { \
      _ ## name ## _ = obj; \
      memory::write_barrier(state, this, obj); \
    } \
    Object** p_ ## name() { \
      return reinterpret_cast<Object**>(&this->_ ## name ## _); \
    }

/**
 *  Create a reader method for a slot.
 *
 *  For attr_reader(foo, SomeClass), creates SomeClass* foo() which returns the
 *  instance variable foo_. A const version is also generated.
 */
#define attr_reader(name, type) \
  private: \
    type* _ ## name ## _; \
  public: \
    type* name() const { return _ ## name ## _; } \
    void name(type* obj) { \
      _ ## name ## _ = obj; \
    } \
    Object** p_ ## name() { \
      return reinterpret_cast<Object**>(&this->_ ## name ## _); \
    }

/**
 *  Ruby-like accessor creation for a slot.
 *
 *  Both attr_writer and attr_reader.
 */
#define attr_accessor(name, type) \
  private: \
    type* _ ## name ## _; \
  public: \
    type* name() const { return _ ## name ## _; } \
    void name(type* obj) { \
      _ ## name ## _ = obj; \
    } \
    template<typename T> \
    void name(T* state, type* obj) { \
      _ ## name ## _ = obj; \
      memory::write_barrier(state, this, obj); \
    } \
    Object** p_ ## name() { \
      return reinterpret_cast<Object**>(&this->_ ## name ## _); \
    }

#define attr_field(name, type) \
  private: \
    type _ ## name ## _; \
  public: \
    type name() const { return _ ## name ## _; } \
    void name(type value) { \
      _ ## name ## _ = value; \
    } \
  Object** p_ ## name() { \
    return reinterpret_cast<Object**>(&this->_ ## name ## _); \
  }
}

#if __GNUC__ >= 4
#define WARN_UNUSED __attribute__ ((warn_unused_result))
#define UNUSED __attribute__((unused))
#else
#define WARN_UNUSED
#define UNUSED
#endif

#ifndef NORETURN
#define NORETURN(x) __attribute__ ((noreturn)) x
#endif

#endif

