#ifndef RBX_BUILTIN_ACCESS_VARIABLE_HPP
#define RBX_BUILTIN_ACCESS_VARIABLE_HPP

#include "class/executable.hpp"

namespace rubinius {

  class InstructionSequence;
  class MachineCode;
  class LexicalScope;

  class AccessVariable : public Executable {
  public:
    const static object_type type = AccessVariableType;

    attr_accessor(name, Symbol);
    attr_accessor(write, Object);

    static void bootstrap(STATE);
    static void initialize(STATE, AccessVariable* av) {
      Executable::initialize(state, av, AccessVariable::access_execute);

      av->name(nil<Symbol>());
      av->write(nil<Object>());
    }

    // Rubinius.primitive :accessvariable_allocate
    static AccessVariable* allocate(STATE);
    static Object* access_execute(STATE, Executable* exec, Module* mod, Arguments& args);
    static Object* access_read_regular_ivar(STATE, Executable* exec, Module* mod, Arguments& args);
    static Object* access_write_regular_ivar(STATE, Executable* exec, Module* mod, Arguments& args);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };
  };
}

#endif
