
#include "type_info.hpp"
#include "builtin/list.hpp"
#include "builtin/string.hpp"
#include "objectmemory.hpp"

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

#include "gen/typechecks.gen.cpp"
}
