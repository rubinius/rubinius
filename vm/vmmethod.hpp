#ifndef RBX_VMMETHOD_HPP
#define RBX_VMMETHOD_HPP

#include <vector>

#include "executor.hpp"
#include "gc/root.hpp"
#include "primitives.hpp"
#include "type_info.hpp"

#include "vm/builtin/compiledmethod.hpp"

#ifdef ENABLE_LLVM
namespace llvm {
  class Function;
}
#endif

namespace rubinius {
  typedef void* instlocation;
  typedef uintptr_t opcode;
  typedef uint32_t bpflags;

  // Breakpoint flags are set in the high byte of an opcode
  const bpflags cBreakpoint = 1 << 24;
  const bpflags cBreakAfterSend = 1 << 25;

  const unsigned int cBreakpointMask = 0x00ffffff;

  class CompiledMethod;
  class MethodContext;
  class Opcode;
  class SendSite;
  class VMMethod;
  class MachineMethod;
  class InterpreterCallFrame;
  class InlineCache;

  typedef Object* (*Runner)(STATE, VMMethod* const vmm, CallFrame* const call_frame);
  typedef Object* (*InterpreterRunner)(STATE, VMMethod* const vmm,
                                       InterpreterCallFrame* const call_frame,
                                       Arguments& args);

  class VMMethod {
  private:
    TypedRoot<MachineMethod*> machine_method_;

  public:
    static void** instructions;
    InterpreterRunner run;

    opcode* opcodes;
    void** addresses;

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
    void* native_function;

    size_t number_of_caches_;
    InlineCache* caches;

  private:
    bool jitted_;

#ifdef ENABLE_LLVM
    llvm::Function* llvm_function_;
    size_t jitted_bytes_;
    void*  jitted_impl_;
#endif

  public: // Methods
    static void init(STATE);

    VMMethod(STATE, CompiledMethod* meth);
    ~VMMethod();

    MachineMethod* machine_method() {
      return machine_method_.get();
    }

    bool jitted() {
      return jitted_;
    }

#ifdef ENABLE_LLVM
    void set_jitted(llvm::Function* func, size_t bytes, void* impl) {
      llvm_function_ = func;
      jitted_impl_ = impl;
      jitted_bytes_ = bytes;
      jitted_ = true;
    }

    llvm::Function* llvm_function() {
      return llvm_function_;
    }

    void* jitted_impl() {
      return jitted_impl_;
    }

    size_t jitted_bytes() {
      return jitted_bytes_;
    }
#endif

    void update_addresses(int index, int operands=0) {
      addresses[index] = instructions[opcodes[index] & cBreakpointMask];
      switch(operands) {
      case 2:
        addresses[index + 2] = reinterpret_cast<void*>(opcodes[index + 2]);
        // fall through
      case 1:
        addresses[index + 1] = reinterpret_cast<void*>(opcodes[index + 1]);
      }
    }

    size_t inline_cache_count() {
      return number_of_caches_;
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

    static Object* uncommon_interpreter(STATE, VMMethod* const vmm,
      CallFrame* const call_frame, Arguments& args, native_int sp);

    void setup_argument_handler(CompiledMethod* meth);

    std::vector<Opcode*> create_opcodes();

    bool validate_ip(STATE, size_t ip);
    void set_breakpoint_flags(STATE, size_t ip, bpflags flags);
    bpflags get_breakpoint_flags(STATE, size_t ip);

    void fill_opcodes(STATE);
    void initialize_caches(STATE, int sends);
    void find_super_instructions();

    void deoptimize(STATE);

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
