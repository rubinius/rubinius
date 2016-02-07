#include "memory.hpp"

#include "builtin/class.hpp"
#include "builtin/diagnostics.hpp"
#include "builtin/lookup_table.hpp"

namespace rubinius {
  void Diagnostics::bootstrap(STATE) {
    Class* diagnostics = state->memory()->new_class<Class, Diagnostics>(
        state, G(rubinius), "Diagnostics");
    diagnostics->set_const(state, "Map", LookupTable::create(state));
  }
}
