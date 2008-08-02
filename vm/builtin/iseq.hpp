#ifndef RBX_BUILTIN_ISEQ_HPP
#define RBX_BUILTIN_ISEQ_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Tuple;

  class InstructionSequence : public Object {
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
      BASIC_TYPEINFO(TypeInfo)
    };

#include "gen/iseq_instruction_names.hpp"
  };

}

#endif
