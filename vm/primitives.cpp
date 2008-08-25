#include "prelude.hpp"
#include "vm.hpp"
#include "primitives.hpp"
#include "event.hpp"
#include "gen/includes.hpp"

#include <iostream>

namespace rubinius {
  bool Primitives::unknown_primitive(STATE, VMExecutable* exec, Task* task, Message& msg) {
    std::cout << "\n";
    state->print_backtrace();
    std::cout << "Called unbound or invalid primitive from: " << msg.name->to_str(state)->byte_address() <<"\n";
    abort();
    return false;
  }

#include "gen/primitives_glue.gen.cpp"
}
