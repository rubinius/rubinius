#ifndef RBX_VMMETHOD_HPP
#define RBX_VMMETHOD_HPP

#include "vmexecutable.hpp"

namespace rubinius {
  typedef void* instlocation;
  typedef uint32_t opcode;

  class VMMethod : public VMExecutable {
  public:
    static instlocation* instructions;

    opcode* opcodes;
    size_t total;
    TypedRoot<CompiledMethod*> original;
    TypeInfo* type;

    VMMethod(STATE, CompiledMethod* meth);
    ~VMMethod();

    void specialize(TypeInfo* ti);
    virtual void execute(STATE, Task* task, Message& msg);
  };
};

#endif
