#include "primitives.hpp"
#include "event.hpp"
#include "gen/includes.hpp"

namespace rubinius {
  bool Primitives::unknown_primitive(STATE, VMExecutable* exec, Task* task, Message& msg) {
    std::cout << "\n";
    state->print_backtrace();
    std::cout << "Called unbound or invalid primitive from: " << *msg.name->to_str(state) <<"\n";
    abort();
    return false;
  }

#include "gen/primitives_glue.gen.cpp"
}
