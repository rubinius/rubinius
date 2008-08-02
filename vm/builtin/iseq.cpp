#include "builtin/iseq.hpp"
#include "builtin/tuple.hpp"

namespace rubinius {
  InstructionSequence* InstructionSequence::create(STATE, size_t instructions) {
    InstructionSequence* is = (InstructionSequence*)state->new_object(G(iseq));
    is->opcodes = Tuple::create(state, instructions);
    return is;
  }

  void InstructionSequence::post_marshal(STATE) {

  }

#include "gen/iseq_instruction_names.cpp"
}
