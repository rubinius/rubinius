#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"

#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include <iostream>

#define StartSize 16
#define Increments 32

namespace rubinius {
  void Symbol::init(STATE) {
    GO(symbol).set(state->new_class("Symbol"));
  }

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
    return state->symbols.lookup_string(state, this);
  }

  const char* Symbol::c_str(STATE) {
    return state->symbols.lookup_cstring(state, this);
  }

  Array* Symbol::all_symbols(STATE) {
    return state->symbols.all_as_array(state);
  }

  void Symbol::Info::show(STATE, OBJECT self, int level) {
    SYMBOL sym = try_as<Symbol>(self);
    std::cout << ":" << sym->to_str(state)->c_str() << std::endl;
  }

  void Symbol::Info::show_simple(STATE, OBJECT self, int level) {
    show(state, self, level);
  }

  void Symbol::Info::mark(OBJECT t, ObjectMark& mark) { }
}
