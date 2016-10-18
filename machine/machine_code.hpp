#ifndef RBX_VMMETHOD_HPP
#define RBX_VMMETHOD_HPP

#include "defines.hpp"
#include "call_frame.hpp"
#include "executor.hpp"
#include "memory/root.hpp"
#include "primitives.hpp"
#include "type_info.hpp"
#include "unwind_info.hpp"
#include "memory.hpp"

#include "class/class.hpp"
#include "class/compiled_code.hpp"
#include "memory/code_resource.hpp"

#include <stdint.h>

namespace rubinius {
  typedef uintptr_t opcode;

  class CompiledCode;
  class CallSite;
  class ConstantCache;
  class MachineCode;

  struct CallFrame;

  typedef Object* (*InterpreterRunner)(STATE, MachineCode* const mcode);

  class MachineCode : public memory::CodeResource {
  public:

    enum Flags {
      eNoInline  = 1 << 0,
      eCompiling = 1 << 1
    };

    const static int cMaxSpecializations = 3;

    struct Specialization {
      ClassData class_data;
      executor execute;
    };

    enum ExecuteStatus {
      eInterpret,
      eJIT,
      eJITDisable,
    };

  public:
    InterpreterRunner run;

    opcode* opcodes;

    size_t total;
    TypeInfo* type;

    bool keywords;

    native_int total_args;
    native_int required_args;
    native_int post_args;
    native_int splat_position;
    native_int keywords_count;

    native_int stack_size;
    native_int number_of_locals;

    native_int iregisters;
    native_int dregisters;

    native_int sample_count;
    native_int call_count;
    native_int uncommon_count;

    attr_field(call_site_count, size_t);
    attr_field(constant_cache_count, size_t);
    attr_field(references_count, size_t);
    attr_field(references, size_t*);
    attr_field(description, std::string*);

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
    static void bootstrap(STATE);

    MachineCode(STATE, CompiledCode* code);
    virtual ~MachineCode();
    virtual void cleanup(STATE, memory::CodeManager* code) {}
    virtual int size();

    bool jitted_p() const {
      return execute_status_ == eJIT;
    }

    bool jit_disabled() const {
      return execute_status_ == eJITDisable;
    }

    bool compiling_p() const {
      return (flags & eCompiling) == eCompiling;
    }

    void set_compiling() {
      flags |= eCompiling;
    }

    void clear_compiling() {
      flags &= ~eCompiling;
    }

    void set_execute_status(ExecuteStatus s) {
      execute_status_ = s;
    }

    Symbol* name() const {
      return name_;
    }

    uint64_t method_id() const {
      return method_id_;
    }

    bool no_inline_p() const {
      return (flags & eNoInline) == eNoInline;
    }

    void set_no_inline() {
      flags |= eNoInline;
    }

    void finalize(STATE);

    CallSite* call_site(STATE, int ip);
    ConstantCache* constant_cache(STATE, int ip);

    Tuple* call_sites(STATE);
    Tuple* constant_caches(STATE);

    void store_call_site(STATE, CompiledCode* code, int ip, CallSite* call_site);
    void store_constant_cache(STATE, CompiledCode* code, int ip, ConstantCache* constant_cache);

    void set_description(STATE);

    void specialize(STATE, CompiledCode* original, TypeInfo* ti);
    static Object* execute(STATE, Executable* exec, Module* mod, Arguments& args);

    template <typename ArgumentHandler>
      static Object* execute_specialized(STATE, Executable* exec, Module* mod, Arguments& args);

    Object* execute_as_script(STATE, CompiledCode* code);

    /**
     *  Interpreting implementation.
     *
     *  @see  machine/instructions.cpp for the code.
     */
    static Object* interpreter(STATE, MachineCode* const mcode);

    static Object* debugger_interpreter(STATE, MachineCode* const mcode);
    static Object* debugger_interpreter_continue(STATE,
                                       MachineCode* const mcode,
                                       CallFrame* const call_frame,
                                       int sp,
                                       InterpreterState& is,
                                       UnwindInfoSet& unwinds);

    static Object* uncommon_interpreter(STATE, MachineCode* const mcode,
      CallFrame* const call_frame, int32_t entry_ip, native_int sp,
      CallFrame* const method_call_frame, UnwindInfoSet& unwinds,
      bool force_deoptimization);

    void setup_argument_handler();

    bool validate_ip(STATE, size_t ip);

    void fill_opcodes(STATE, CompiledCode* original);

    void deoptimize(STATE, CompiledCode* original, bool disable=false);

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

      size_t position() const {
        return position_;
      }

      size_t next_position() const {
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

      bool last_instruction() const {
        return next_position() >= size_;
      }

      opcode op() const {
        return stream_[position_] & 0x00ffff;
      }

      opcode operand1() const {
        return stream_[position_ + 1];
      }

      opcode operand2() const {
        return stream_[position_ + 2];
      }

      int next_pos() const {
        return position_ + width();
      }

      opcode next() const {
        return stream_[next_pos()];
      }

      size_t width() const {
        opcode op = this->op();
#include "gen/instruction_sizes.hpp"
        return width;
      }

      size_t args() const {
        return width() - 1;
      }

      bool end() const {
        return position_ >= size_;
      }
    };

  };
};

#endif
