#ifndef RBX_VMMETHOD_HPP
#define RBX_VMMETHOD_HPP

#include "executor.hpp"
#include "gc/root.hpp"
#include "primitives.hpp"
#include "type_info.hpp"
#include "unwind_info.hpp"

#include "vm/builtin/compiledcode.hpp"
#include "gc/code_resource.hpp"

#ifdef ENABLE_LLVM
namespace llvm {
  class Function;
}
#endif

namespace rubinius {
  typedef uintptr_t opcode;

  class CompiledCode;
  class MachineCode;
  class InterpreterCallFrame;
  class InlineCache;

  typedef Object* (*InterpreterRunner)(STATE, MachineCode* const mcode,
                                       InterpreterCallFrame* const call_frame);

  class MachineCode : public CodeResource {
  public:
    static void** instructions;

    enum Flags {
      eNoInline = 1 << 0
    };

    const static int cMaxSpecializations = 3;

    struct Specialization {
      int class_id;
      executor execute;
      jit::RuntimeDataHolder* jit_data;
    };

    enum ExecuteStatus {
      eInterpret,
      eJIT,
      eJITDisable,
    };

  private:
    MachineCode* parent_;

  public:
    InterpreterRunner run;

    opcode* opcodes;
    void** addresses;

    std::size_t total;
    TypeInfo* type;

    native_int total_args;
    native_int required_args;
    native_int post_args;
    native_int splat_position;

    native_int stack_size;
    native_int number_of_locals;

    native_int call_count;
    native_int uncommon_count;

    size_t number_of_caches_;
    InlineCache* caches;

    Specialization specializations[cMaxSpecializations];
    executor unspecialized;
    executor fallback;

  private:
    ExecuteStatus execute_status_;

    Symbol* name_;
    uint64_t method_id_;
  public:
    uint32_t debugging;

  private:
    uint32_t flags; // Used to store bit flags
  public: // Methods
    static void init(STATE);

    MachineCode(STATE, CompiledCode* meth);
    virtual ~MachineCode();
    virtual void cleanup(STATE, CodeManager* code);
    virtual int size();

    bool jitted() {
      return execute_status_ == eJIT;
    }

    bool jit_disabled() {
      return execute_status_ == eJITDisable;
    }

    void set_execute_status(ExecuteStatus s) {
      execute_status_ = s;
    }

    void update_addresses(int index, int operands=0) {
      addresses[index] = instructions[opcodes[index]];
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

    MachineCode* parent() {
      return parent_;
    }

    void set_parent(MachineCode* parent) {
      parent_ = parent;
    }

    bool for_block() {
      return parent_ != 0;
    }

    Symbol* name() {
      return name_;
    }

    uint64_t method_id() {
      return method_id_;
    }

    bool no_inline_p() {
      return (flags & eNoInline) == eNoInline;
    }

    void set_no_inline() {
      flags |= eNoInline;
    }

    void specialize(STATE, CompiledCode* original, TypeInfo* ti);
    void compile(STATE);
    static Object* execute(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);

    template <typename ArgumentHandler>
      static Object* execute_specialized(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);

    Object* execute_as_script(STATE, CompiledCode* code, CallFrame* previous);

    struct InterpreterState {
      int call_flags;

      InterpreterState()
        : call_flags(0)
      {}
    };

    /**
     *  Interpreting implementation.
     *
     *  @see  vm/instructions.cpp for the code.
     */
    static Object* interpreter(STATE, MachineCode* const mcode,
                               InterpreterCallFrame* const call_frame);

    static Object* debugger_interpreter(STATE, MachineCode* const mcode,
                                        InterpreterCallFrame* const call_frame);
    static Object* debugger_interpreter_continue(STATE,
                                       MachineCode* const mcode,
                                       CallFrame* const call_frame,
                                       int sp,
                                       InterpreterState& is,
                                       int current_unwind,
                                       UnwindInfo* unwinds);

    static Object* uncommon_interpreter(STATE, MachineCode* const mcode,
      CallFrame* const call_frame, int32_t entry_ip, native_int sp,
      CallFrame* const method_call_frame, jit::RuntimeDataHolder* rd,
      int32_t unwind_count, int32_t* unwinds);

    static Object* tooling_interpreter(STATE,
                                       MachineCode* const mcode,
                                       InterpreterCallFrame* const call_frame);

    void setup_argument_handler(CompiledCode* meth);

    bool validate_ip(STATE, size_t ip);

    void fill_opcodes(STATE, CompiledCode* original);
    void initialize_caches(STATE, CompiledCode* original, int sends);
    void find_super_instructions();

    void deoptimize(STATE, CompiledCode* original, jit::RuntimeDataHolder* rd,
                    bool disable=false);

    /*
     * Helper class for iterating over an Opcode array.  Used to convert a
     * MachineCode to an LLVM method.
     */
    class Iterator {
    public:
      opcode* stream_;
      size_t position_;
      size_t size_;

      Iterator(MachineCode* mcode)
        : stream_(mcode->opcodes)
        , position_(0)
        , size_(mcode->total)
      {}

      Iterator(opcode* stream, size_t size)
        : stream_(stream)
        , position_(0)
        , size_(size)
      {}

      size_t position() {
        return position_;
      }

      size_t next_position() {
        return position_ + width();
      }

      void inc() {
        position_ += width();
      }

      bool advance() {
        size_t next = next_position();
        if(next >= size_) return false;

        position_ = next;
        return true;
      }

      bool last_instruction() {
        return next_position() >= size_;
      }

      opcode op() {
        return stream_[position_] & 0x00ffff;
      }

      opcode operand1() {
        return stream_[position_ + 1];
      }

      opcode operand2() {
        return stream_[position_ + 2];
      }

      int next_pos() {
        return position_ + width();
      }

      opcode next() {
        return stream_[next_pos()];
      }

      size_t width() {
        opcode op = this->op();
#include "gen/instruction_sizes.hpp"
        return width;
      }

      size_t args() {
        return width() - 1;
      }

      bool end() {
        return position_ >= size_;
      }
    };

  };
};

#endif
