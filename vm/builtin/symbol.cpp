#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "object_utils.hpp"
#include "objectmemory.hpp"
#include "ontology.hpp"

#include <sstream>

#define StartSize 16
#define Increments 32

namespace rubinius {
  void Symbol::init(STATE) {
    GO(symbol).set(ontology::new_class(state, "Symbol"));
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
    String* str = state->shared().symbols.lookup_string(state, this);
    if(!str) {
      std::ostringstream msg;
      msg << "Invalid symbol 0x" << std::hex << reinterpret_cast<uintptr_t>(this);
      Exception::range_error(state, msg.str().c_str());
    }
    return str;
  }

  std::string& Symbol::cpp_str(STATE) {
    return state->shared().symbols.lookup_cppstring(this);
  }

  std::string Symbol::debug_str(SharedState& shared) {
    return shared.symbols.lookup_debug_string(this);
  }

  std::string Symbol::debug_str(STATE) {
    return debug_str(state->shared());
  }

  Array* Symbol::all_symbols(STATE) {
    return state->shared().symbols.all_as_array(state);
  }

  Object* Symbol::is_ivar_p(STATE) {
    return RBOOL(state->shared().symbols.kind(state, this) == SymbolTable::IVar);
  }

  Object* Symbol::is_cvar_p(STATE) {
    return RBOOL(state->shared().symbols.kind(state, this) == SymbolTable::CVar);
  }

  Object* Symbol::is_constant_p(STATE) {
    return RBOOL(state->shared().symbols.kind(state, this) == SymbolTable::Constant);
  }

  Encoding* Symbol::encoding(STATE) {
    return Encoding::from_index(state, state->shared().symbols.lookup_encoding(this));
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
