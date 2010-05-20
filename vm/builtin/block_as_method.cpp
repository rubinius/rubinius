#include "builtin/block_as_method.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/system.hpp"
#include "builtin/class.hpp"
#include "builtin/location.hpp"

#include "dispatch.hpp"
#include "call_frame.hpp"
#include "arguments.hpp"

#include "object_utils.hpp"

namespace rubinius {
  BlockAsMethod* BlockAsMethod::create(STATE, Object* self, BlockEnvironment* be) {
    BlockAsMethod* pe = state->new_object<BlockAsMethod>(as<Class>(self));
    pe->block_env(state, be);

    pe->execute = block_executor;
    return pe;
  }

  Object* BlockAsMethod::block_executor(STATE, CallFrame* call_frame, Dispatch& msg,
                                       Arguments& args)
  {
    BlockAsMethod* bm = as<BlockAsMethod>(msg.method);

    int required = bm->block_env()->method()->required_args()->to_native();

    // Stupidly, if 0 or 1 args are required by the proc, we still
    // go ahead and call it.
    //
    // Calling with 0, args are ignored.
    // Calling with 1, args are put into an array and the local gets the array
    //   (this is handled by the bytecode prologue of the block)
    // Otherwise, it's strict arg checking.
    //
    if(required > 1 && (size_t)required != args.total()) {
      Exception* exc =
        Exception::make_argument_error(state, required, args.total(), msg.name);
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    BlockInvocation invocation(args.recv(),
        bm->block_env()->method()->scope(),
        CallFrame::cIsLambda | CallFrame::cBlockAsMethod);

    invocation.module = msg.module;

    return bm->block_env()->invoke(state, call_frame,
                                   bm->block_env(), args, invocation);
  }
}
