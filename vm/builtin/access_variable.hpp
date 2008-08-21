#ifndef RBX_BUILTIN_ACCESS_VARIABLE_HPP
#define RBX_BUILTIN_ACCESS_VARIABLE_HPP

#include "builtin/executable.hpp"
#include "vm.hpp"

namespace rubinius {

  class InstructionSequence;
  class MemoryPointer;
  class VMMethod;
  class StaticScope;

  class AccessVariable : public Executable {
  public:
    const static size_t fields = Executable::fields + 2;
    const static object_type type = AccessVariableType;

    SYMBOL name;  // slot
    OBJECT write; // slot

    static void init(STATE);
    // Ruby.primitive :accessvariable_allocate
    static AccessVariable* allocate(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
