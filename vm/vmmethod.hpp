#ifndef RBX_VMMETHOD_HPP
#define RBX_VMMETHOD_HPP

#include "vmexecutable.hpp"
#include "primitives.hpp"

namespace rubinius {
  typedef void* instlocation;
  typedef uint32_t opcode;

  class Opcode;

  class VMMethod : public VMExecutable {
  public:
    static instlocation* instructions;

    opcode* opcodes;
    size_t total;
    TypedRoot<CompiledMethod*> original;
    TypeInfo* type;
    std::vector<VMMethod*> blocks;

    VMMethod(STATE, CompiledMethod* meth);
    ~VMMethod();

    virtual void specialize(TypeInfo* ti);
    virtual void compile();
    static bool executor(STATE, VMExecutable* meth, Task* task, Message& msg);
    virtual void resume(Task* task, MethodContext* ctx);

    std::vector<Opcode*> create_opcodes();

    class Iterator {
    public:
      VMMethod* vmm;
      size_t position;

      Iterator(VMMethod* vmm) :
        vmm(vmm), position(0) { }

      void inc() {
        position += width();
      }

      opcode op() {
        return vmm->opcodes[position];
      }

      int operand1() {
        return vmm->opcodes[position + 1];
      }

      int operand2() {
        return vmm->opcodes[position + 2];
      }

      size_t next_pos() {
        return position + width();
      }

      opcode next() {
        return vmm->opcodes[next_pos()];
      }

      size_t width() {
        opcode op = this->op();
#include "gen/iseq_instruction_size.gen"
        return width;
      }

      size_t args() {
        return width() - 1;
      }

      bool end() {
        return position >= vmm->total;
      }
    };

  };

  class Opcode {
  public:
    opcode op;
    int args;
    int arg1;
    int arg2;
    bool start_block;
    size_t block;

    Opcode(opcode op, int o1 = -1, int o2 = -1) :
      op(op), args(0), arg1(o1), arg2(o2), start_block(false), block(NULL) {
        if(o1 >= 0) args++;
        if(o2 >= 0) args++;
      }

    Opcode(VMMethod::Iterator& iter) : start_block(false), block(NULL) {
      op = iter.op();
      args = iter.args();

      switch(args) {
      case 2:
        arg2 = iter.operand2();
      case 1:
        arg1 = iter.operand1();
      }

    }

    bool is_goto();
    bool is_terminator();
    bool is_send();
  };

  class VMPrimitiveMethod : public VMMethod {
  public:
    VMPrimitiveMethod(STATE, CompiledMethod* meth, rubinius::executor func);
  };
};

#endif
