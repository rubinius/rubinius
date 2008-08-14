#include "builtin/symbol.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "builtin/tuple.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"

#define StartSize 16
#define Increments 32

namespace rubinius {

  native_int Symbol::index() {
    return DATA_STRIP_TAG(this);
  }

  INTEGER Symbol::index(STATE) {
    return Fixnum::from(this->index());
  }

  Symbol* Symbol::from_index(STATE, size_t index) {
    return (Symbol*)DATA_APPLY_TAG(index, DATA_TAG_SYMBOL);
  }

  String* Symbol::to_str(STATE) {
    return state->symbol_to_string(this);
  }

  void Symbol::Info::mark(OBJECT t, ObjectMark& mark) { }
}
