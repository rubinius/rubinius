#include "defines.hpp"
#include "thread_state.hpp"
#include "primitives.hpp"
#include "includes.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"

#include <iostream>

namespace rubinius {

extern "C" Object* invoke_unknown_primitive(STATE,
    Object** args, int arg_count)
{
  Exception::internal_error(state, "unable to invoke primitive");
  return 0;
}

#include "invoke_primitives.hpp"
#include "invoke_resolver.hpp"
}
