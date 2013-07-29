#include "prelude.hpp"
#include "vm.hpp"
#include "primitives.hpp"
#include "gen/includes.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"
#include "on_stack.hpp"

#include "instruments/tooling.hpp"
#include "dtrace/dtrace.h"

#include <iostream>

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

namespace rubinius {

  Object* Primitives::unknown_primitive(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args) {
    std::string message = std::string("Called unbound or invalid primitive from method name: ");
    message += args.name()->to_str(state)->c_str(state);

    Exception::assertion_error(state, message.c_str());

    return cNil;
  }

#include "gen/method_primitives.cpp"
#include "gen/method_resolver.cpp"
}
