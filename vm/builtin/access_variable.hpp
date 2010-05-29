#ifndef RBX_BUILTIN_ACCESS_VARIABLE_HPP
#define RBX_BUILTIN_ACCESS_VARIABLE_HPP

#include "builtin/executable.hpp"
#include "vm.hpp"

namespace rubinius {

  class InstructionSequence;
  class VMMethod;
  class StaticScope;

  class AccessVariable : public Executable {
  public:
    const static object_type type = AccessVariableType;

  private:
    Symbol* name_;  // slot
    Object* write_; // slot

  public:
    /* accessors */

    attr_accessor(name, Symbol);
    attr_accessor(write, Object);

    /* interface */

    static void init(STATE);
    // Ruby.primitive :accessvariable_allocate
    static AccessVariable* allocate(STATE);
    static Object* access_execute(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args);
    static Object* access_read_regular_ivar(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args);
    static Object* access_write_regular_ivar(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };
  };
}

#endif
