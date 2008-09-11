#include "builtin/class.hpp"
#include "builtin/methodvisibility.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/executable.hpp"
#include "builtin/symbol.hpp"
#include "vm.hpp"

#include <iostream>

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
