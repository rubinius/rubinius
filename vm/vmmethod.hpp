#ifndef RBX_VMMETHOD_HPP
#define RBX_VMMETHOD_HPP

#include <vector>

#include "executor.hpp"
#include "gc/root.hpp"
#include "primitives.hpp"
#include "type_info.hpp"
#include "unwind_info.hpp"

#include "vm/builtin/compiledmethod.hpp"
#include "gc/code_resource.hpp"

#ifdef ENABLE_LLVM
namespace llvm {
  class Function;
}
#endif

namespace rubinius {
  typedef void* instlocation;
  typedef uintptr_t opcode;
  typedef uint32_t bpflags;

  typedef std::list<Object**> IndirectLiterals;

  // Breakpoint flags are set in the high byte of an opcode
  const bpflags cBreakpoint = 1 << 24;
  const bpflags cBreakAfterSend = 1 << 25;

  const unsigned int cBreakpointMask = 0x00ffffff;

  class CompiledMethod;
  class MethodContext;
  class SendSite;
  class VMMethod;
  class InterpreterCallFrame;
  class InlineCache;

  typedef Object* (*Runner)(STATE, VMMethod* const vmm, CallFrame* const call_frame);
  typedef Object* (*InterpreterRunner)(STATE, VMMethod* const vmm,
                                       InterpreterCallFrame* const call_frame);

  class VMMethod : public CodeResource {
  public:
    static void** instructions;

  private:
    IndirectLiterals indirect_literals_;
    VMMethod* parent_;

  public:
    InterpreterRunner run;

    opcode* opcodes;
    void** addresses;

    std::size_t total;
    TypeInfo* type;

    native_int total_args;
    native_int required_args;
    native_int splat_position;

    native_int stack_size;
    native_int number_of_locals;

    native_int call_count;
    native_int uncommon_count;

    size_t number_of_caches_;
    InlineCache* caches;

#ifdef ENABLE_LLVM
  private:
    llvm::Function* llvm_function_;
    size_t jitted_bytes_;
    void*  jitted_impl_;
#endif

    Symbol* name_;
    uint64_t method_id_;
  public:
    bool debugging;
  public: // Methods
    static void init(STATE);

    VMMethod(STATE, CompiledMethod* meth);
    virtual ~VMMethod();
    virtual void cleanup(CodeManager* cm);
    virtual int size();

#ifdef ENABLE_LLVM
    bool jitted() {
      return jitted_impl_ != 0;
    }

    void set_jitted(llvm::Function* func, size_t bytes, void* impl) {
      llvm_function_ = func;
      jitted_impl_ = impl;
      jitted_bytes_ = bytes;
    }

    void* native_function() {
      return jitted_impl_;
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
#else
    bool jitted() {
      return false;
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

    IndirectLiterals& indirect_literals() {
      return indirect_literals_;
    }

    VMMethod* parent() {
      return parent_;
    }

    void set_parent(VMMethod* parent) {
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

    void specialize(STATE, CompiledMethod* original, TypeInfo* ti);
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
                                   InterpreterCallFrame* const call_frame);

    /**
     *  Interpreting implementation.
     *
     *  @see  vm/llvm/instructions.cpp for the code.
     */
    static Object* interpreter(STATE, VMMethod* const vmm,
                               InterpreterCallFrame* const call_frame);

    static Object* debugger_interpreter(STATE, VMMethod* const vmm,
                                        InterpreterCallFrame* const call_frame);
    static Object* debugger_interpreter_continue(STATE,
                                       VMMethod* const vmm,
                                       CallFrame* const call_frame,
                                       int sp,
                                       InterpreterState& is,
                                       int current_unwind,
                                       UnwindInfo* unwinds);

    static Object* uncommon_interpreter(STATE, VMMethod* const vmm,
      CallFrame* const call_frame, int32_t entry_ip, native_int sp,
      CallFrame* const method_call_frame,
      int32_t unwind_count, int32_t* unwinds);

    void setup_argument_handler(CompiledMethod* meth);

    bool validate_ip(STATE, size_t ip);
    void set_breakpoint_flags(STATE, size_t ip, bpflags flags);
    bpflags get_breakpoint_flags(STATE, size_t ip);

    void fill_opcodes(STATE, CompiledMethod* original);
    void initialize_caches(STATE, CompiledMethod* original, int sends);
    void find_super_instructions();

    void deoptimize(STATE, CompiledMethod* original);

    /*
     * Helper class for iterating over an Opcode array.  Used to convert a
     * VMMethod to an LLVM method.
     */
    class Iterator {
    public:
      opcode* stream_;
      size_t position_;
      size_t size_;

      Iterator(VMMethod* vmm)
        : stream_(vmm->opcodes)
        , position_(0)
        , size_(vmm->total)
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
