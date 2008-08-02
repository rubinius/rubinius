#include "builtin/methodtable.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  MethodTable* MethodTable::create(STATE) {
    MethodTable *tbl;

    tbl = (MethodTable*)state->om->new_object(G(lookuptable),
                                              LookupTable::fields);
    tbl->setup(state, 0);

    return tbl;
  }
}
