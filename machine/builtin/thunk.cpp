#include "arguments.hpp"
#include "memory.hpp"
#include "object_utils.hpp"

#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/location.hpp"
#include "builtin/system.hpp"
#include "builtin/thunk.hpp"

namespace rubinius {
  Thunk* Thunk::create(STATE, Object* self, Object* value) {
    Thunk* pe = state->memory()->new_object<Thunk>(state, as<Class>(self));

    pe->value(state, value);
    pe->execute = thunk_executor;

    return pe;
  }

  Object* Thunk::thunk_executor(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
                                       Arguments& args)
  {
    Thunk* thunk = as<Thunk>(exec);

    if(args.total() != 0) {
      Exception* exc =
        Exception::make_argument_error(state, 0, args.total(), args.name());
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->raise_exception(exc);
      return NULL;
    }

    return thunk->value();
  }
}
