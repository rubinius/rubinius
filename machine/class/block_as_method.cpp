#include "arguments.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "object_utils.hpp"
#include "memory.hpp"

#include "class/block_as_method.hpp"
#include "class/block_environment.hpp"
#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/location.hpp"

namespace rubinius {
  BlockAsMethod* BlockAsMethod::create(STATE, Object* self, BlockEnvironment* be) {
    BlockAsMethod* pe = state->memory()->new_object<BlockAsMethod>(state, as<Class>(self));
    pe->block_env(state, be);
    pe->inliners(0);
    pe->prim_index(-1);
    pe->custom_call_site(false);
    pe->execute = block_executor;
    return pe;
  }

  Object* BlockAsMethod::block_executor(STATE, Executable* exec, Module* mod,
                                       Arguments& args)
  {
    BlockAsMethod* bm = as<BlockAsMethod>(exec);

    Fixnum* splat = bm->block_env()->compiled_code()->splat();
    size_t required = bm->block_env()->compiled_code()->required_args()->to_native();
    size_t total_args = bm->block_env()->compiled_code()->total_args()->to_native();

    /*
     * These are the block shapes, required args, and splat that we may see,
     * along with the arity check that we must perform:
     *
     *   block shape   required args   splat   check
     *  -------------|---------------|-------|-------
     *  { || }       |  0            |  nil  |  ==
     *  {  }         |  0            |  -2   |  none
     *  { |a| }      |  1            |  nil  |  none (1.8), == (>= 1.9)
     *  { |*a| }     |  0            |  0    |  none
     *  { |a, b| }   |  2            |  nil  |  ==
     *  { |a, *b| }  |  1            |  1    |  >=
     *
     * NOTE that when taking one argument, any arguments passed are put
     * into an array and the local gets the array (or an empty array if
     * no arguments are passed). This is handled by the bytecode prologue
     * of the block.
     */
    bool exception = false;
    size_t expected = 0;
    if(splat->nil_p()) {
      if(args.total() > total_args) {
        exception = true;
        expected = total_args;
      }
      if(args.total() < required) {
        exception = true;
        expected = required;
      }
    } else {
      if(required > args.total()) {
        exception = true;
        expected = required;
      }
    }

    if(exception) {
      Exception* exc =
        Exception::make_argument_error(state, expected, args.total(),
            args.name()->cpp_str(state).c_str());
      exc->locations(state, Location::from_call_stack(state));
      state->raise_exception(exc);
      return NULL;
    }

    BlockInvocation invocation(args.recv(),
        bm->block_env()->lexical_scope(),
        CallFrame::cIsLambda | CallFrame::cBlockAsMethod);

    invocation.module = mod;

    return bm->block_env()->invoke(state,
                                   bm->block_env(), args, invocation);
  }
}
