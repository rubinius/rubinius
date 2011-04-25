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

    Object* splat = bm->block_env()->code()->splat();
    int required = bm->block_env()->code()->required_args()->to_native();

    /*
     * These are the block shapes, required args, and splat that we may see,
     * along with the arity check that we must perform:
     *
     *   block shape   required args   splat   check
     *  -------------|---------------|-------|-------
     *  { || }       |  0            |  nil  |  ==
     *  {  }         |  0            |  -2   |  none
     *  { |a| }      |  1            |  nil  |  none
     *  { |*a| }     |  0            |  0    |  none
     *  { |a, b| }   |  2            |  nil  |  ==
     *  { |a, *b| }  |  1            |  1    |  >=
     *
     * NOTE that when taking one argument, any arguments passed are put
     * into an array and the local gets the array (or an empty array if
     * no arguments are passed). This is handled by the bytecode prologue
     * of the block.
     */
    if((splat->nil_p() && (required == 0 || required > 1)
            && (size_t)required != args.total())
        || (!splat->nil_p() && required > 0 && (size_t)required > args.total())) {
      Exception* exc =
        Exception::make_argument_error(state, required, args.total(), msg.name);
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    BlockInvocation invocation(args.recv(),
        bm->block_env()->code()->scope(),
        CallFrame::cIsLambda | CallFrame::cBlockAsMethod);

    invocation.module = msg.module;

    return bm->block_env()->invoke(state, call_frame,
                                   bm->block_env(), args, invocation);
  }
}
