#include "builtin/methodtable.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  MethodTable* MethodTable::create(STATE) {
    MethodTable *tbl;

    tbl = (MethodTable*)state->new_object(G(methtbl));
    tbl->setup(state, 0);

    return tbl;
  }

  MethodTable* MethodTable::allocate(STATE) {
    return create(state);
  }
}
