
#include "primitives.hpp"
#include "event.hpp"

#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {
  bool Primitives::unknown_primitive(STATE, VMExecutable* exec, Task* task, Message& msg) {
    std::cout << "Called unbound/invalid primitive!\n";
    abort();
  }

#include "gen/primitives_glue.gen.cpp"
}
