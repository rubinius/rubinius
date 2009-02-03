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
    const static object_type type = ExecutableType;

  private:
    Symbol* primitive_; // slot
    Fixnum* serial_;    // slot

  public:
    // This one is public so it can be directly invoked.
    executor execute;

    /* accessors */

    attr_accessor(primitive, Symbol);
    attr_accessor(serial, Fixnum);

    /* interface */

    static void init(STATE);
    static Object* default_executor(STATE, CallFrame* call_frame, Task* task, Message& msg);

    // Ruby.primitive :executable_allocate
    static Executable* allocate(STATE, Object* self);

    void set_executor(rubinius::executor exc) {
      execute = exc;
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}

#endif
