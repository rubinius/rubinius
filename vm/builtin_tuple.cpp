#include "prelude.hpp"
#include "vm.hpp"
#include "objects.hpp"
#include "objectmemory.hpp"

#include <cstdarg>

namespace rubinius {
  Tuple* Tuple::create(STATE, size_t fields) {
    return (Tuple*)state->om->new_object(state->globals.tuple, fields);
  }

  Tuple* Tuple::from(STATE, size_t fields, ...) {
    va_list ar;
    Tuple* tup = create(state, fields);

    va_start(ar, fields);
    for(size_t i = 0; i < fields; i++) {
      tup->put(state, i, va_arg(ar, OBJECT));
    }
    va_end(ar);

    return tup;
  }

  void Tuple::copy_from(STATE, Tuple* other, int start, int end) {

  }

  OBJECT Tuple::put(STATE, size_t idx, OBJECT val) {
    state->om->store_object(this, idx, val);
    return val;
  }

}
