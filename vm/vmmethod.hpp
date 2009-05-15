#ifndef RBX_VMMETHOD_HPP
#define RBX_VMMETHOD_HPP

#include <vector>

#include "executor.hpp"
#include "gc/root.hpp"
#include "primitives.hpp"
#include "type_info.hpp"

#include "vm/builtin/compiledmethod.hpp"

namespace rubinius {
  typedef void* instlocation;
  typedef uintptr_t opcode;
  typedef uint32_t bpflags;

  // Breakpoint flags are set in the high byte of an opcode
  const bpflags cBreakpoint = 1 << 24;
  const bpflags cBreakAfterSend = 1 << 25;

  class CompiledMethod;
  class MethodContext;
  class Opcode;
  class SendSite;
  class VMMethod;
  class MachineMethod;
  class InterpreterCallFrame;

  typedef Object* (*Runner)(STATE, VMMethod* const vmm, CallFrame* const call_frame);
  typedef Object* (*InterpreterRunner)(STATE, VMMethod* const vmm,
                                       InterpreterCallFrame* const call_frame,
                                       Arguments& args);

  class VMMethod {
  private:
    TypedRoot<MachineMethod*> machine_method_;

  public:
    static instlocation* instructions;
    InterpreterRunner run;

    opcode* opcodes;
    std::size_t total;
    TypedRoot<CompiledMethod*> original;
    TypeInfo* type;
    std::vector<VMMethod*> blocks;

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
    static Object* execute(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args);

    template <typename ArgumentHandler>
      static Object* execute_specialized(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args);

    struct InterpreterState {
      bool allow_private;
      int call_flags;

      InterpreterState()
        : allow_private(false)
        , call_flags(0)
      {}
    };

    /**
     *  Dispatch method on the defined interpreter.
     */
    static Object* run_interpreter(STATE, VMMethod* const vmm,
                                   InterpreterCallFrame* const call_frame,
                                   Arguments& args);

    /**
     *  Interpreting implementation.
     *
     *  @see  vm/llvm/instructions.cpp for the code.
     */
    static Object* interpreter(STATE, VMMethod* const vmm,
                               InterpreterCallFrame* const call_frame,
                               Arguments& args);

    static Object* debugger_interpreter(STATE, VMMethod* const vmm,
                                        InterpreterCallFrame* const call_frame,
                                        Arguments& args);

    void setup_argument_handler(CompiledMethod* meth);

    std::vector<Opcode*> create_opcodes();

    bool validate_ip(STATE, size_t ip);
    void set_breakpoint_flags(STATE, size_t ip, bpflags flags);
    bpflags get_breakpoint_flags(STATE, size_t ip);

    void fill_opcodes(STATE);
    void find_super_instructions();

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
