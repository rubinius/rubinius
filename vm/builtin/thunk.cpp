#include "builtin/thunk.hpp"
#include "builtin/system.hpp"
#include "builtin/class.hpp"
#include "builtin/location.hpp"

#include "dispatch.hpp"
#include "call_frame.hpp"
#include "arguments.hpp"

#include "object_utils.hpp"

namespace rubinius {
  Thunk* Thunk::create(STATE, Object* self, Object* value) {
    Thunk* pe = state->new_object<Thunk>(as<Class>(self));
    pe->value(state, value);

    pe->execute = thunk_executor;
    return pe;
  }

  Object* Thunk::thunk_executor(STATE, CallFrame* call_frame, Dispatch& msg,
                                       Arguments& args)
  {
    Thunk* thunk = as<Thunk>(msg.method);

    if(args.total() != 0) {
      Exception* exc =
        Exception::make_argument_error(state, 0, args.total(), msg.name);
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    return thunk->value();
  }
}
