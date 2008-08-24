#include "prelude.hpp"
#include "vm.hpp"

#include "builtin/class.hpp"
#include "builtin/executable.hpp"

namespace rubinius {
  void Executable::init(STATE) {
    GO(executable).set(state->new_class("Executable", G(object), Executable::fields));
  }
}
