#include "builtin/class.hpp"
#include "builtin/diagnostics.hpp"
#include "builtin/lookup_table.hpp"

namespace rubinius {
  void Diagnostics::init(STATE) {
    Module* diagnostics = state->new_object<Diagnostics>(G(module));
    diagnostics->setup(state, "Diagnostics", G(rubinius));

    diagnostics->set_const(state, "Map", LookupTable::create(state));
  }
}
