#include "builtin.hpp"

namespace rubinius {
  ISeq* ISeq::create(STATE, size_t bytes) {
    return (ISeq*)state->om->new_object_bytes(G(iseq), bytes);
  }

  void ISeq::post_marshal(STATE) {

  }
}
