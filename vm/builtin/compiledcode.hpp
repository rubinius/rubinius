#ifndef RBX_BUILTIN_COMPILEDCODE_HPP
#define RBX_BUILTIN_COMPILEDCODE_HPP

#include "builtin/executable.hpp"

namespace rubinius {

  class InstructionSequence;
  class MachineCode;
  class ConstantScope;
  class CallSite;

  namespace jit {
    class RuntimeDataHolder;
  }

  class CompiledCode : public Executable {
  public:
    const static object_type type = CompiledCodeType;

  private:
    Object* metadata_;          // slot
    Symbol* name_;              // slot
    InstructionSequence* iseq_; // slot
    Fixnum* stack_size_;        // slot
    Fixnum* local_count_;       // slot
    Fixnum* required_args_;     // slot
    Fixnum* post_args_;         // slot
    Fixnum* total_args_;        // slot
    Fixnum* splat_;             // slot
    Tuple* lines_;              // slot
    Tuple* local_names_;        // slot
    Symbol* file_;              // slot
    ConstantScope* scope_;      // slot
    LookupTable* breakpoints_;  // slot

    MachineCode* machine_code_;

#ifdef ENABLE_LLVM
    jit::RuntimeDataHolder* jit_data_;
#endif

  public:
    // Access directly from assembly, so has to be public.
    Tuple* literals_;           // slot

    /* accessors */
    MachineCode* machine_code() {
      return machine_code_;
    }

#ifdef ENABLE_LLVM
    jit::RuntimeDataHolder* jit_data() {
      return jit_data_;
    }

    void set_jit_data(jit::RuntimeDataHolder* rds) {
      jit_data_ = rds;
    }
#endif

    bool can_specialize_p();
    void set_unspecialized(executor exec, jit::RuntimeDataHolder* rd);
    void add_specialized(uint32_t class_id, uint32_t serial_id, executor exec, jit::RuntimeDataHolder* rd);
    executor find_specialized(Class* cls);

    attr_accessor(metadata, Object);
    attr_accessor(name, Symbol);
    attr_accessor(iseq, InstructionSequence);
    attr_accessor(stack_size, Fixnum);
    attr_accessor(local_count, Fixnum);
    attr_accessor(required_args, Fixnum);
    attr_accessor(post_args, Fixnum);
    attr_accessor(total_args, Fixnum);
    attr_accessor(splat, Fixnum);
    attr_accessor(literals, Tuple);
    attr_accessor(lines, Tuple);
    attr_accessor(local_names, Tuple);
    attr_accessor(file, Symbol);
    attr_accessor(scope, ConstantScope);
    attr_accessor(breakpoints, LookupTable);

    /* interface */

    static void init(STATE);

    // Rubinius.primitive :compiledcode_allocate
    static CompiledCode* create(STATE);

    static Object* primitive_failed(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);

    int start_line(STATE);
    int start_line();
    int line(STATE, int ip);
    int line(int ip);

    void post_marshal(STATE);
    size_t number_of_locals();
    MachineCode* internalize(STATE, GCToken gct, CallFrame* call_frame, const char** failure_reason=0, int* ip=0);
    void specialize(STATE, TypeInfo* ti);

    static Object* default_executor(STATE, CallFrame*, Executable* exec, Module* mod, Arguments& args);
    static Object* specialized_executor(STATE, CallFrame*, Executable* exec, Module* mod, Arguments& args);

    // Rubinius.primitive :compiledcode_set_breakpoint
    Object* set_breakpoint(STATE, GCToken gct, Fixnum* ip, Object* bp, CallFrame* calling_environment);

    // Rubinius.primitive :compiledcode_clear_breakpoint
    Object* clear_breakpoint(STATE, Fixnum* ip);

    // Rubinius.primitive :compiledcode_is_breakpoint
    Object* is_breakpoint(STATE, Fixnum* ip);

    // Rubinius.primitive+ :compiledcode_of_sender
    static CompiledCode* of_sender(STATE, CallFrame* calling_environment);

    // Rubinius.primitive+ :compiledcode_current
    static CompiledCode* current(STATE, CallFrame* calling_environment);

    // Rubinius.primitive :compiledcode_dup
    CompiledCode* dup(STATE);

    // Rubinius.primitive :compiledcode_call_sites
    Tuple* call_sites(STATE, CallFrame* calling_environment);

    // Rubinius.primitive :compiledcode_constant_caches
    Tuple* constant_caches(STATE, CallFrame* calling_environment);

    String* full_name(STATE);

    bool kernel_method(STATE);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
      virtual void mark(Object* obj, ObjectMark& mark);
      virtual void show(STATE, Object* self, int level);
    };

    friend class Info;
  };

};


#endif
