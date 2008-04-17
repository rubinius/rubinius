#include "objects.hpp"

namespace rubinius {
  CompiledMethod* CompiledMethod::create(STATE) {
    return (CompiledMethod*)state->new_object(G(cmethod));
  }

  void CompiledMethod::post_marshal(STATE) {

  }

  size_t CompiledMethod::stack_size() {
    return 0;
  }

  size_t CompiledMethod::number_of_locals() {
    return 0;
  }
}
