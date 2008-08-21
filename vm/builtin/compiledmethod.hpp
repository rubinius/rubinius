#ifndef RBX_BUILTIN_COMPILEDMETHOD_HPP
#define RBX_BUILTIN_COMPILEDMETHOD_HPP

#include "builtin/executable.hpp"
#include "vm.hpp"

namespace rubinius {

  class InstructionSequence;
  class MemoryPointer;
  class VMMethod;
  class StaticScope;

  class CompiledMethod : public Executable {
    public:
    const static size_t fields = 18;
    const static object_type type = CMethodType;
    const static size_t saved_fields = 16;

    SYMBOL name; // slot
    InstructionSequence*  iseq; // slot
    FIXNUM stack_size; // slot
    FIXNUM local_count; // slot
    FIXNUM required_args; // slot
    FIXNUM total_args; // slot
    OBJECT splat; // slot
    Tuple* literals; // slot
    Tuple* exceptions; // slot
    Tuple* lines; // slot
    Tuple* local_names; // slot
    SYMBOL file; // slot
    StaticScope* scope; // slot

    static CompiledMethod* create(STATE);
    // Use a stack of 1 so that the return value of the executed method
    // has a place to go
    const static size_t tramp_stack_size = 1;
    static CompiledMethod* generate_tramp(STATE, size_t stack_size = tramp_stack_size);

    void post_marshal(STATE);
    size_t number_of_locals();
    void set_scope(StaticScope*);
    VMMethod* formalize(STATE, bool ondemand=true);
    void specialize(TypeInfo* ti);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class MethodVisibility : public Object {
  public:
    const static size_t fields = 2;
    const static object_type type = CMVisibilityType;

    SYMBOL visibility; // slot
    Executable* method; // slot

    static MethodVisibility* create(STATE);

    bool public_p(STATE) {
      return visibility == G(sym_public);
    }

    bool private_p(STATE) {
      return visibility == G(sym_private);
    }

    bool protected_p(STATE) {
      return visibility == G(sym_protected);
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

};


#endif
