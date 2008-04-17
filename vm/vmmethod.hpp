#ifndef RBX_VMMETHOD_HPP
#define RBX_VMMETHOD_HPP

namespace rubinius {
  typedef void* instlocation;
  typedef uint32_t opcode;

  class VMMethod {
  public:
    static instlocation* instructions;

    opcode* opcodes;
    CompiledMethod* original;

    VMMethod(CompiledMethod* meth);
    ~VMMethod();
  };
};

#endif
