#ifndef RBX_BUILTIN_ISEQ_HPP
#define RBX_BUILTIN_ISEQ_HPP

#include "objects.hpp"

namespace rubinius {
  class InstructionSequence : public BuiltinType {
  public:
    const static size_t fields = 3;
    const static object_type type = ISeqType;

    OBJECT __ivars__; // slot
    Tuple* opcodes; // slot
    FIXNUM stack_depth; // slot

    void post_marshal(STATE);
    static InstructionSequence* create(STATE, size_t instructions);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
    };

#include "gen/iseq_instruction_names.hpp"
  };
}

#endif
