#ifndef RBX_BUILTIN_COMPILEDCODE_HPP
#define RBX_BUILTIN_COMPILEDCODE_HPP

#include "object_utils.hpp"
#include "memory.hpp"
#include "spinlock.hpp"

#include "class/lexical_scope.hpp"
#include "class/executable.hpp"
#include "class/fixnum.hpp"
#include "class/iseq.hpp"
#include "class/lookup_table.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"

namespace rubinius {

  class InstructionSequence;
  class MachineCode;
  class LexicalScope;
  class CallSite;

  namespace jit {
    class RuntimeDataHolder;
  }

  class CompiledCode : public Executable {
  public:
    const static object_type type = CompiledCodeType;

    attr_accessor(metadata, Object);
    attr_accessor(name, Symbol);
    attr_accessor(iseq, InstructionSequence);
    attr_accessor(stack_size, Fixnum);
    attr_accessor(local_count, Fixnum);
    attr_accessor(required_args, Fixnum);
    attr_accessor(post_args, Fixnum);
    attr_accessor(total_args, Fixnum);
    attr_accessor(splat, Fixnum);
    attr_accessor(lines, Tuple);
    attr_accessor(local_names, Tuple);
    attr_accessor(file, Symbol);
    attr_accessor(scope, LexicalScope);
    attr_accessor(keywords, Tuple);
    attr_accessor(arity, Fixnum);
    attr_accessor(breakpoints, LookupTable);
    attr_accessor(registers, Fixnum);
    attr_accessor(code_id, String);

  private:
    attr_field(machine_code, MachineCode*);
    locks::spinlock_mutex _lock_;

  public:
    attr_accessor(literals, Tuple)

    /* interface */

    static void bootstrap(STATE);
    static void finalize(STATE, CompiledCode* code);
    static void initialize(STATE, CompiledCode* obj) {
      Executable::initialize(state, obj, CompiledCode::default_executor);

      obj->metadata(nil<Object>());
      obj->name(nil<Symbol>());
      obj->iseq(nil<InstructionSequence>());
      obj->stack_size(nil<Fixnum>());
      obj->local_count(Fixnum::from(0));
      obj->required_args(nil<Fixnum>());
      obj->post_args(nil<Fixnum>());
      obj->total_args(nil<Fixnum>());
      obj->splat(nil<Fixnum>());
      obj->lines(nil<Tuple>());
      obj->local_names(nil<Tuple>());
      obj->file(nil<Symbol>());
      obj->scope(nil<LexicalScope>());
      obj->keywords(nil<Tuple>());
      obj->arity(nil<Fixnum>());
      obj->breakpoints(nil<LookupTable>());
      obj->registers(Fixnum::from(0));
      obj->code_id(nil<String>());
      obj->machine_code(NULL);

      obj->literals(nil<Tuple>());

      obj->_lock_.reset();
    }

    static CompiledCode* create(STATE);

    // Rubinius.primitive :compiledcode_allocate
    static CompiledCode* allocate(STATE, Object* self);

    static Object* primitive_failed(STATE, Executable* exec, Module* mod, Arguments& args);

    locks::spinlock_mutex& lock() {
      return _lock_;
    }

    int start_line(STATE);
    int start_line();
    int line(STATE, int ip);
    int line(int ip);

    void post_marshal(STATE);
    size_t number_of_locals();
    MachineCode* internalize(STATE);
    void specialize(STATE, TypeInfo* ti);

    static Object* default_executor(STATE, Executable* exec, Module* mod, Arguments& args);

    // Rubinius.primitive :compiledcode_set_breakpoint
    Object* set_breakpoint(STATE, Fixnum* ip, Object* bp);

    // Rubinius.primitive :compiledcode_clear_breakpoint
    Object* clear_breakpoint(STATE, Fixnum* ip);

    // Rubinius.primitive :compiledcode_is_breakpoint
    Object* is_breakpoint(STATE, Fixnum* ip);

    // Rubinius.primitive+ :compiledcode_of_sender
    static CompiledCode* of_sender(STATE);

    // Rubinius.primitive+ :compiledcode_current
    static CompiledCode* current(STATE);

    // Rubinius.primitive :compiledcode_dup
    CompiledCode* dup(STATE);

    // Rubinius.primitive :compiledcode_call_sites
    Tuple* call_sites(STATE);

    // Rubinius.primitive :compiledcode_constant_caches
    Tuple* constant_caches(STATE);

    // Rubinius.primitive :compiledcode_jitted_p
    Object* jitted_p(STATE);

    // Rubinius.primitive :compiledcode_sample_count
    Fixnum* sample_count(STATE);

    // Rubinius.primitive :compiledcode_stamp_id
    String* stamp_id(STATE);

    String* full_name(STATE);

    bool core_method(STATE);

    Object* execute_script(STATE);

    uint32_t nil_id(STATE);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
      virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
      virtual void show(STATE, Object* self, int level);
    };

    friend class Info;
  };

};


#endif
