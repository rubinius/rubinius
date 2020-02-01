#include "defines.hpp"
#include "thread_state.hpp"
#include "primitives.hpp"
#include "includes.hpp"
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

#include "method_primitives.hpp"
#include "method_resolver.hpp"
}
