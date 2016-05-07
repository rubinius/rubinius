#include "defines.hpp"
#include "vm.hpp"
#include "state.hpp"
#include "primitives.hpp"
#include "gen/includes.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"
#include "on_stack.hpp"

#include "dtrace/dtrace.h"

#include <iostream>

namespace rubinius {

  Object* Primitives::unknown_primitive(STATE, Executable* exec, Module* mod, Arguments& args) {
    std::string message = std::string("Called unbound or invalid primitive from method name: ");
    message += args.name()->to_str(state)->c_str(state);

    Exception::raise_assertion_error(state, message.c_str());

    return cNil;
  }

#include "gen/method_primitives.cpp"
#include "gen/method_resolver.cpp"
}
