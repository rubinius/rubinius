#include "prelude.hpp"
#include "vm.hpp"
#include "primitives.hpp"
#include "gen/includes.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"

#include "instruments/tooling.hpp"

#include <iostream>

namespace rubinius {

extern "C" Object* invoke_unknown_primitive(STATE, CallFrame* call_frame,
    Object** args, int arg_count)
{
  Exception::internal_error(state, call_frame, "unable to invoke primitive");
  return 0;
}

#include "gen/invoke_primitives.cpp"
#include "gen/invoke_resolver.cpp"
}
