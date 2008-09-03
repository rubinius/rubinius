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

    std::cout << "#<" << self->class_object(state)->name->c_str(state) <<
      ":" << (void*) self << "\n";
    indent(++level); std::cout << "visibility: "; mv->visibility->show(state, level);
    indent(level); std::cout << "method: "; mv->method->show(state, level);
    indent(--level); std::cout << ">\n";
  }
};
