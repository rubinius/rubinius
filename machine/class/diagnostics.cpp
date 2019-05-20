#include "memory.hpp"

#include "class/class.hpp"
#include "class/diagnostics.hpp"
#include "class/lookup_table.hpp"

namespace rubinius {
  void Diagnostics::bootstrap(STATE) {
    state->memory()->new_class<Class, Diagnostics>(
        state, G(rubinius), "Diagnostics");
  }
}
