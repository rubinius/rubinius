#include "objects.hpp"

namespace rubinius {
  CompiledMethod* CompiledMethod::create(STATE) {
    return (CompiledMethod*)state->new_object(G(cmethod));
  }

  CompiledMethod::Visibility* CompiledMethod::Visibility::create(STATE) {
    return (CompiledMethod::Visibility*)state->new_object(G(cmethod_vis));
  }

  void CompiledMethod::post_marshal(STATE) {

  }

  size_t CompiledMethod::number_of_locals() {
    return 0;
  }
}
