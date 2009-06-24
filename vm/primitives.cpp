#include "prelude.hpp"
#include "vm.hpp"
#include "primitives.hpp"
#include "event.hpp"
#include "gen/includes.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"

#include "instruments/profiler.hpp"

#include <iostream>

namespace rubinius {
  Object* Primitives::unknown_primitive(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args) {
    std::string message = std::string("Called unbound or invalid primitive from method name: ");
    message += msg.name->to_str(state)->c_str();

    Exception::assertion_error(state, message.c_str());

    return Qnil;
  }

#include "gen/primitives_glue.gen.cpp"
}
