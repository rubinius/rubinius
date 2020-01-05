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

#include <atomic>
#include <stdint.h>

namespace rubinius {
  namespace diagnostics {
    class Measurement;
  }

  typedef uintptr_t opcode;

  class CompiledCode;
  class CallSite;
  class ConstantCache;
  class MachineCode;
  class UnwindSite;

  struct CallFrame;

  typedef Object* (*InterpreterRunner)(STATE, MachineCode* const mcode);

  class MachineCode : public memory::CodeResource {
  public:

    enum Flags {
      eNoInline  = 1 << 0,
      eCompiling = 1 << 1
    };

    enum ExecuteStatus {
      eInterpret,
      eJIT,
      eJITDisable,
    };

    static std::atomic<uint64_t> code_serial;

  public:
    InterpreterRunner run;

    opcode* opcodes;

    size_t total;
    TypeInfo* type;

    bool keywords;

    intptr_t total_args;
    intptr_t required_args;
    intptr_t post_args;
    intptr_t splat_position;
    intptr_t keywords_count;

    intptr_t stack_size;
    intptr_t number_of_locals;

    intptr_t registers;

    intptr_t sample_count;
    intptr_t call_count;
    intptr_t uncommon_count;

    attr_field(call_site_count, size_t);
    attr_field(constant_cache_count, size_t);
    attr_field(references_count, size_t);
    attr_field(references, size_t*);
    attr_field(unwind_site_count, size_t);
    attr_field(serial, uint64_t);
    attr_field(nil_id, uint32_t);

    std::string _name_;
    std::string _location_;

    executor unspecialized;
    executor fallback;

  private:
    ExecuteStatus execute_status_;

  public:
    uint32_t debugging;

  private:
    uint32_t flags; // Used to store bit flags
  public: // Methods
    static uint64_t get_serial() {
      return ++code_serial;
    }

    static void bootstrap(STATE);

    static MachineCode* create(STATE, CompiledCode* code);

    MachineCode(STATE, CompiledCode* code);
    virtual ~MachineCode();
    virtual void cleanup(STATE, memory::CodeManager* code) {}
    virtual int size();

    const std::string& name() const {
      return _name_;
    }

    const std::string& location() const {
      return _location_;
    }

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
    void store_unwind_site(STATE, CompiledCode* code, int ip, UnwindSite* unwind_site);
    void store_measurement(STATE, CompiledCode* code, int ip, diagnostics::Measurement* m);

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
      CallFrame* const call_frame, int32_t entry_ip, intptr_t sp,
      CallFrame* const method_call_frame, UnwindInfoSet& unwinds,
      bool force_deoptimization);

    void setup_argument_handler();
  };
};

#endif
