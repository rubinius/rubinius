#include "builtin/methodvisibility.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/executable.hpp"
#include "vm.hpp"

namespace rubinius {

  MethodVisibility* MethodVisibility::create(STATE) {
    return (MethodVisibility*)state->new_object(G(cmethod_vis));
  }

};
