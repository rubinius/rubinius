
#include "type_info.hpp"
#include "objectmemory.hpp"
#include "gen/includes.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {

  TypeInfo::~TypeInfo() { }

  TypeInfo::TypeInfo(object_type type) : type(type) {
    state = NULL;
  }

  void TypeInfo::cleanup(OBJECT obj) { }

  void TypeInfo::set_field(STATE, OBJECT target, size_t index, OBJECT val) {
    throw std::runtime_error("field access denied");
  }

  OBJECT TypeInfo::get_field(STATE, OBJECT target, size_t index) {
    throw std::runtime_error("unable to access field");
  }

  void TypeInfo::mark(OBJECT obj, ObjectMark& mark) {
    throw std::runtime_error("unable to mark object");
  }

  void TypeInfo::show(STATE, OBJECT self) {
    if(FIXNUM i = try_as<Fixnum>(self)) {
      std::cout << i->n2i() << std::endl;
    } else if(Bignum* b = try_as<Bignum>(self)) {
      std::cout << *b->to_s(state, Object::i2n(10)) << std::endl;
    } else if(Float* f = try_as<Float>(self)) {
      std::cout << f->val << std::endl;
    } else if(String* str = try_as<String>(self)) {
      std::cout << *str << std::endl;
    } else if(SYMBOL sym = try_as<Symbol>(self)) {
      std::cout << ":" << *sym->to_str(state) << std::endl;
    } else {
      inspect(state, self);
    }
  }
#include "gen/typechecks.gen.cpp"

  /* For use in gdb. */
  extern "C" {
    /* A wrapper because gdb can't do virtual dispatch. */
    void __show__(STATE, OBJECT obj) {
      obj->show(state);
    }
  }
}
