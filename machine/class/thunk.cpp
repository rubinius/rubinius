#include "arguments.hpp"
#include "memory.hpp"
#include "object_utils.hpp"

#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/location.hpp"
#include "class/system.hpp"
#include "class/thunk.hpp"

namespace rubinius {
  Thunk* Thunk::create(STATE, Object* self, Object* value) {
    Thunk* pe = state->memory()->new_object<Thunk>(state, as<Class>(self));

    pe->value(state, value);
    pe->execute = thunk_executor;

    return pe;
  }

  Object* Thunk::thunk_executor(STATE, Executable* exec, Module* mod,
                                       Arguments& args)
  {
    Thunk* thunk = as<Thunk>(exec);

    if(args.total() != 0) {
      Exception* exc =
        Exception::make_argument_error(state, 0,
            args.total(), args.name()->cpp_str(state).c_str());
      exc->locations(state, Location::from_call_stack(state));
      state->raise_exception(exc);
      return NULL;
    }

    return thunk->value();
  }
}
