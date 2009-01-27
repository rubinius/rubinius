#ifndef RBX_VMMETHOD_HPP
#define RBX_VMMETHOD_HPP

#include <vector>

#include "executor.hpp"
#include "gc_root.hpp"
#include "primitives.hpp"
#include "type_info.hpp"

namespace rubinius {
  typedef void* instlocation;
  typedef uint32_t opcode;
  typedef uint32_t bpflags;

  // Breakpoint flags are set in the high byte of an opcode
  const bpflags cBreakpoint = 1 << 24;
  const bpflags cBreakAfterSend = 1 << 25;

  class CompiledMethod;
  class MethodContext;
  class Opcode;
  class SendSite;
  class VMMethod;
  class Task;
  class MachineMethod;

  typedef void (*Runner)(VMMethod* const vmm, Task* const task, MethodContext* const ctx);

  class VMMethod {
  private:
    TypedRoot<MachineMethod*> machine_method_;

  public:
    static instlocation* instructions;

    // To run this method, we execute this function pointer
    Runner run;

    opcode* opcodes;
    std::size_t total;
    TypedRoot<CompiledMethod*> original;
    TypeInfo* type;
    std::vector<VMMethod*> blocks;
    TypedRoot<SendSite*> *sendsites;

    native_int total_args;
    native_int required_args;
    native_int splat_position;

    native_int stack_size;
    native_int number_of_locals;

    native_int call_count;

  public: // Methods
    static void init(STATE);

    VMMethod(STATE, CompiledMethod* meth);
    ~VMMethod();

    MachineMethod* machine_method() {
      return machine_method_.get();
    }

    void set_machine_method(MachineMethod* mm);

    void specialize(STATE, TypeInfo* ti);
    void compile(STATE);
    static ExecuteStatus execute(STATE, Task* task, Message& msg);

    template <typename ArgumentHandler>
      static ExecuteStatus execute_specialized(STATE, Task* task, Message& msg);

    static void run_interpreter(VMMethod* const vmm, Task* const task, MethodContext* const ctx);
    static void interpreter(VMMethod* const vmm, Task* const task, MethodContext* const ctx);
    static void debugger_interpreter(VMMethod* const vmm, Task* const task, MethodContext* const ctx);

    void setup_argument_handler(CompiledMethod* meth);

    std::vector<Opcode*> create_opcodes();

    bool validate_ip(STATE, size_t ip);
    void set_breakpoint_flags(STATE, size_t ip, bpflags flags);
    bpflags get_breakpoint_flags(STATE, size_t ip);

    /*
     * Helper class for iterating over an Opcode array.  Used to convert a
     * VMMethod to an LLVM method.
     */
    class Iterator {
    public:
      VMMethod* vmm;
      std::size_t position;

      Iterator(VMMethod* vmm) :
        vmm(vmm), position(0) { }

      void inc() {
        position += width();
      }

      opcode op() {
        return vmm->opcodes[position] & 0x00ffff;
      }

      int operand1() {
        return vmm->opcodes[position + 1];
      }

      int operand2() {
        return vmm->opcodes[position + 2];
      }

      std::size_t next_pos() {
        return position + width();
      }

      opcode next() {
        return vmm->opcodes[next_pos()];
      }

      std::size_t width() {
        opcode op = this->op();
#include "gen/iseq_instruction_size.gen"
        return width;
      }

      std::size_t args() {
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
    std::size_t block;

    Opcode(opcode op, int o1 = -1, int o2 = -1) :
      op(op), args(0), arg1(o1), arg2(o2), start_block(false), block(0) {
        if(o1 >= 0) args++;
        if(o2 >= 0) args++;
      }

    Opcode(VMMethod::Iterator& iter) : start_block(false), block(0) {
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
};

#endif
