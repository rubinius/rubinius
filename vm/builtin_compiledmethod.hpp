#ifndef RBX_BUILTIN_COMPILEDMETHOD_HPP
#define RBX_BUILTIN_COMPILEDMETHOD_HPP

namespace rubinius {

  class InstructionSequence;
  class MemoryPointer;
  class VMMethod;

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
    void post_marshal(STATE);
    size_t number_of_locals();
    void set_scope(StaticScope*);
    VMMethod* vmmethod(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
    };
  };

  class MethodVisibility : public BuiltinType {
  public:
    const static size_t fields = 3;
    const static object_type type = CMVisibilityType;

    OBJECT __ivars__; // slot
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
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
    };
  };

  template <>
    static bool kind_of<Executable>(OBJECT obj) {
      if(obj->obj_type == Executable::type ||
         obj->obj_type == CompiledMethod::type) {
        return true;
      }

      return false;
    }
};


#endif
