#include "prelude.hpp"
#include "vm.hpp"

#include "builtin/class.hpp"
#include "builtin/executable.hpp"

namespace rubinius {
  template <>
    bool kind_of<Executable>(OBJECT obj) {
      if(obj->obj_type == Executable::type ||
         obj->obj_type == AccessVariableType ||
         obj->obj_type == CMethodType ||
         obj->obj_type == NativeFuncType ||
         obj->obj_type == NMethodType) {
        return true;
      }

      return false;
    }

  void Executable::init(STATE) {
    GO(executable).set(state->new_class("Executable", G(object), Executable::fields));
  }
}
