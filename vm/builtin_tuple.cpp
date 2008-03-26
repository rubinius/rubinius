#include "prelude.hpp"
#include "vm.hpp"
#include "objects.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  Tuple* Tuple::create(STATE, size_t fields) {
    return (Tuple*)state->om->new_object(state->globals.tuple, fields);
  }

  OBJECT Tuple::put(STATE, size_t idx, OBJECT val) {
    state->om->store_object(this, idx, val);
    return val;
  }

}
