/* A BlockEnvironment is created when a block is created. Its primary
 * operation is call, which activates the code associated with the block. */

#include "objects.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  void BlockEnvironment::call(STATE, Task* task, size_t args) {
    OBJECT val;
    if(args > 0) {
      Tuple* tup = Tuple::create(state, args);
      for(size_t i = args - 1; i >= 0; i--) {
        tup->put(state, i, task->pop());
      }

      val = tup;
    } else {
      val = Qnil;
    }
    task->pop(); // Remove this from the stack.
    BlockContext* ctx = create_context(state, task->active);
    task->make_active(ctx);
    task->push(val);
  }

  void BlockEnvironment::call(STATE, Message& msg) {
    throw std::runtime_error("call2: not implemented");
  }

  BlockContext* BlockEnvironment::create_context(STATE, MethodContext* sender) {
    BlockContext* ctx = BlockContext::create(state);
    SET(ctx, sender, sender);
    SET(ctx, name, (SYMBOL)this);
    SET(ctx, cm, method);
    SET(ctx, stack, Tuple::create(state, method->stack_size->to_nint() + 1));
    SET(ctx, home, home);

    ctx->vmm = vmm;
    ctx->ip = 0;
    ctx->sp = -1;

    return ctx;
  }

  BlockEnvironment* BlockEnvironment::under_context(STATE, CompiledMethod* cm,
      MethodContext* parent, MethodContext* active, size_t index) {

    BlockEnvironment* be = (BlockEnvironment*)state->new_object(G(blokenv));


    VMMethod* vmm;
    if((vmm = active->vmm->blocks[index]) == NULL) {
      vmm = new VMMethod(state, cm);
      vmm->specialize(active->vmm->type);
      active->vmm->blocks[index] = vmm;
    }

    SET(be, home, parent);
    SET(be, home_block, active);
    SET(be, method, cm);
    SET(be, local_count, cm->local_count);
    be->vmm = vmm;

    return be;
  }
}
