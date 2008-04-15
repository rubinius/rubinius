#include "objects.hpp"

namespace rubinius {
  CompiledMethod* CompiledMethod::create(STATE) {
    return (CompiledMethod*)state->new_object(G(cmethod));
  }

  void CompiledMethod::post_marshal(STATE) {

  }
}
