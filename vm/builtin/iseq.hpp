#ifndef RBX_BUILTIN_ISEQ_HPP
#define RBX_BUILTIN_ISEQ_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class Tuple;

  class InstructionSequence : public Object {
  public:
    const static object_type type = InstructionSequenceType;

  private:
    Tuple* opcodes_;     // slot

  public:
    /* accessors */

    attr_accessor(opcodes, Tuple);

    /* interface */

    static void init(STATE);
    static InstructionSequence* create(STATE, size_t instructions);

    static size_t instruction_width(size_t op);

    void post_marshal(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

#include "gen/instruction_names.hpp"
  };

}

#endif
