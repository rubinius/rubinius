#include "builtin/class.hpp"
#include "builtin/iseq.hpp"
#include "builtin/tuple.hpp"
#include "ontology.hpp"

namespace rubinius {
  void InstructionSequence::init(STATE) {
    GO(iseq).set(ontology::new_class(state,
          "InstructionSequence", G(object), G(rubinius)));
    G(iseq)->set_object_type(state, InstructionSequenceType);
  }

  InstructionSequence* InstructionSequence::create(STATE, size_t instructions) {
    InstructionSequence* is = state->new_object<InstructionSequence>(G(iseq));
    is->opcodes(state, Tuple::create(state, instructions));
    return is;
  }

  size_t InstructionSequence::instruction_width(size_t op) {
#include "gen/instruction_sizes.hpp"
    return width;
  }

  void InstructionSequence::post_marshal(STATE) {

  }

#include "gen/instruction_names.cpp"
}
