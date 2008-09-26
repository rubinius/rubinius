#include "vm/vm.hpp"
#include "vm/object.hpp"

#include "builtin/methodvisibility.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {

  MethodVisibility* MethodVisibility::create(STATE) {
    return (MethodVisibility*)state->new_object(G(cmethod_vis));
  }

  void MethodVisibility::Info::show(STATE, OBJECT self, int level) {
    MethodVisibility* mv = as<MethodVisibility>(self);

    class_header(state, self);
    indent_attribute(++level, "visibility"); mv->visibility()->show(state, level);
    indent_attribute(level, "method"); mv->method()->show(state, level);
    close_body(level);
  }
};
