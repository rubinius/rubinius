#include "builtin/class.hpp"
#include "builtin/iseq.hpp"
#include "builtin/tuple.hpp"

#include "vm.hpp"

namespace rubinius {
  void InstructionSequence::init(STATE) {
    GO(iseq).set(state->new_class("InstructionSequence", G(object), InstructionSequence::fields));
    G(iseq)->set_object_type(state, InstructionSequenceType);
  }

  InstructionSequence* InstructionSequence::create(STATE, size_t instructions) {
    InstructionSequence* is = (InstructionSequence*)state->new_object(G(iseq));
    is->opcodes(state, Tuple::create(state, instructions));
    return is;
  }

  size_t InstructionSequence::instruction_width(size_t op) {
#include "gen/iseq_instruction_size.gen"
    return width;
  }

  void InstructionSequence::post_marshal(STATE) {

  }

#include "gen/iseq_instruction_names.cpp"
}
