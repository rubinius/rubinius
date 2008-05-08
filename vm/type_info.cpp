#include "builtin.hpp"
#include "type_info.hpp"

namespace rubinius {
  TypeInfo::TypeInfo(object_type type) : type(type) {
    state = NULL;
  }

  void TypeInfo::set_field(STATE, OBJECT target, size_t index, OBJECT val) {
    throw std::runtime_error("field access denied");
  }

  OBJECT TypeInfo::get_field(STATE, OBJECT target, size_t index) {
    throw std::runtime_error("unable to access field");
  }

#include "gen/typechecks.gen.cpp"
}
