#ifndef RBX_BUILTIN_ISEQ_HPP
#define RBX_BUILTIN_ISEQ_HPP

#include "object_utils.hpp"

#include "class/tuple.hpp"
#include "class/object.hpp"

namespace rubinius {
  class InstructionSequence : public Object {
  public:
    const static object_type type = InstructionSequenceType;

    attr_accessor(opcodes, Tuple);

    /* interface */

    static void bootstrap(STATE);
    static void initialize(STATE, InstructionSequence* obj) {
      obj->opcodes(nil<Tuple>());
    }

    static InstructionSequence* create(STATE, size_t instructions);

    static size_t instruction_width(size_t op);

    // Rubinius.primitive :instruction_sequence_allocate
    static InstructionSequence* allocate(STATE, Object* self);

    void post_marshal(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };
}

#endif
