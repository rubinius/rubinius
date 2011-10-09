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
    G(symbol)->set_object_type(state, Symbol::type);
  }

  native_int Symbol::index() const {
    return STRIP_SYMBOL_TAG(this);
  }

  Integer* Symbol::index(STATE) {
    return Fixnum::from(this->index());
  }

  Symbol* Symbol::from_index(STATE, size_t index) {
    return (Symbol*)APPLY_SYMBOL_TAG(index);
  }

  String* Symbol::to_str(STATE) {
    return state->shared.symbols.lookup_string(state, this);
  }

  std::string& Symbol::cpp_str(STATE) {
    return state->shared.symbols.lookup_cppstring(this);
  }

  std::string Symbol::debug_str(STATE) {
    return state->shared.symbols.lookup_debug_string(this);
  }

  Array* Symbol::all_symbols(STATE) {
    return state->shared.symbols.all_as_array(state);
  }

  Object* Symbol::is_ivar_p(STATE) {
    return state->shared.symbols.kind(state, this) == SymbolTable::IVar ? Qtrue : Qfalse;
  }

  Object* Symbol::is_cvar_p(STATE) {
    return state->shared.symbols.kind(state, this) == SymbolTable::CVar ? Qtrue : Qfalse;
  }

  Object* Symbol::is_constant_p(STATE) {
    return state->shared.symbols.kind(state, this) == SymbolTable::Constant ? Qtrue : Qfalse;
  }

  void Symbol::Info::show(STATE, Object* self, int level) {
    Symbol* sym = try_as<Symbol>(self);
    std::cout << ":" << sym->debug_str(state) << std::endl;
  }

  void Symbol::Info::show_simple(STATE, Object* self, int level) {
    show(state, self, level);
  }

  void Symbol::Info::mark(Object* t, ObjectMark& mark) { }
}
