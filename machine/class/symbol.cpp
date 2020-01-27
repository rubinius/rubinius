#include "class/array.hpp"
#include "class/class.hpp"
#include "class/encoding.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/tuple.hpp"
#include "object_utils.hpp"
#include "memory.hpp"

#include <sstream>

#define StartSize 16
#define Increments 32

namespace rubinius {
  void Symbol::bootstrap(STATE) {
    GO(symbol).set(state->memory()->new_class<Class, Symbol>(state, "Symbol"));
  }

  intptr_t Symbol::index() const {
    return STRIP_SYMBOL_TAG(this);
  }

  Integer* Symbol::index(STATE) {
    return Fixnum::from(this->index());
  }

  Symbol* Symbol::from_index(STATE, size_t index) {
    return (Symbol*)APPLY_SYMBOL_TAG(index);
  }

  String* Symbol::to_str(STATE) {
    String* str = state->memory()->symbols.lookup_string(state, this);
    if(!str) {
      std::ostringstream msg;
      msg << "Invalid symbol 0x" << std::hex << reinterpret_cast<uintptr_t>(this);
      Exception::raise_range_error(state, msg.str().c_str());
    }
    return str;
  }

  std::string& Symbol::cpp_str(STATE) {
    return state->memory()->symbols.lookup_cppstring(this);
  }

  std::string Symbol::debug_str(STATE) {
    return state->memory()->symbols.lookup_debug_string(this);
  }

  Array* Symbol::all_symbols(STATE) {
    return state->memory()->symbols.all_as_array(state);
  }

  Object* Symbol::is_ivar_p(STATE) {
    return RBOOL(state->memory()->symbols.kind(state, this) == SymbolTable::eIVar);
  }

  Object* Symbol::is_cvar_p(STATE) {
    return RBOOL(state->memory()->symbols.kind(state, this) == SymbolTable::eCVar);
  }

  Object* Symbol::is_constant_p(STATE) {
    return RBOOL(state->memory()->symbols.kind(state, this) == SymbolTable::eConstant);
  }

  Encoding* Symbol::encoding(STATE) {
    return Encoding::from_index(state, state->memory()->symbols.lookup_encoding(this));
  }

  void Symbol::Info::show(STATE, Object* self, int level) {
    Symbol* sym = try_as<Symbol>(self);
    std::cout << ":" << sym->debug_str(state) << std::endl;
  }

  void Symbol::Info::show_simple(STATE, Object* self, int level) {
    show(state, self, level);
  }

  void Symbol::Info::mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) { }
}
