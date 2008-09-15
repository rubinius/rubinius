#include "gc_root.hpp"
#include "vm.hpp"

namespace rubinius {
  Root::Root(STATE) : object(NULL), roots(&state->globals.roots) { }

  Root::Root(STATE, OBJECT obj) : object(NULL), roots(NULL) {
    set(obj, &state->globals.roots);
  }

  Root* Roots::front() {
    return static_cast<Root*>(head());
  }
}
