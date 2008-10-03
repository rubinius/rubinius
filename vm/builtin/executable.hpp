#ifndef RBX_BUILTIN_EXECUTABLE_HPP
#define RBX_BUILTIN_EXECUTABLE_HPP

#include <cassert>

#include "builtin/object.hpp"
#include "type_info.hpp"
#include "executor.hpp"

namespace rubinius {
  class MemoryPointer;

  class Executable : public Object {
  public:
    const static size_t fields = 3;
    const static object_type type = ExecutableType;

  private:
    SYMBOL primitive_; // slot
    FIXNUM serial_;    // slot

    // TODO: fix up data members that aren't slots
    executor execute_;

  public:
    /* accessors */

    attr_accessor(primitive, Symbol);
    attr_accessor(serial, Fixnum);

    /* interface */

    static void init(STATE);
    static bool default_executor(STATE, Executable* exc, Task* task, Message& msg);

    // Ruby.primitive :executable_allocate
    static Executable* allocate(STATE, OBJECT self);

    /* Perform the actions of this Executable object. */
    bool execute(STATE, Task* task, Message& msg) {
      assert(execute_ && (OBJECT)execute_ != Qnil);
      return execute_(state, this, task, msg);
    }

    void set_executor(rubinius::executor exc) {
      execute_ = exc;
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}

#endif
