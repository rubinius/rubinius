#include "builtin.hpp"

namespace rubinius {
  ISeq* ISeq::create(STATE, size_t instructions) {
    ISeq* is = (ISeq*)state->new_object(G(iseq));
    is->instructions = Tuple::create(state, instructions);
    return is;
  }

  void ISeq::post_marshal(STATE) {

  }

#include "gen/iseq_instruction_names.cpp"
}
