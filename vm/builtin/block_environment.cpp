/* A BlockEnvironment is created when a block is created. Its primary
 * operation is call, which activates the code associated with the block. */

#include "builtin/object.hpp"
#include "builtin/symbol.hpp"

#include "block_environment.hpp"
#include "objectmemory.hpp"

#include "vm/object_utils.hpp"

#include "profiler.hpp"
#include "message.hpp"

#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/contexts.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/task.hpp"
#include "builtin/tuple.hpp"

#include <iostream>

namespace rubinius {

  void BlockEnvironment::init(STATE) {
    GO(blokenv).set(state->new_class("BlockEnvironment", G(object)));
    G(blokenv)->set_object_type(state, BlockEnvironmentType);
  }

  BlockEnvironment* BlockEnvironment::allocate(STATE) {
    BlockEnvironment* env = state->new_object<BlockEnvironment>(G(blokenv));
    env->vmm = NULL;
    return env;
  }

  void BlockEnvironment::call(STATE, Task* task, size_t args) {
    Object* val;
    if(args > 0) {
      Tuple* tup = Tuple::create(state, args);
      for(int i = args - 1; i >= 0; i--) {
        tup->put(state, i, task->pop());
      }

      val = tup;
    } else {
      val = Qnil;
    }
    task->pop(); // Remove this from the stack.
    BlockContext* ctx = create_context(state, task->active());

    if(unlikely(task->profiler)) task->profiler->enter_block(state, home_, method_);

    task->make_active(ctx);
    task->push(val);
    ctx->run(ctx->vmm, task, ctx);
  }

  void BlockEnvironment::call(STATE, Task* task, Message& msg) {
    Object* val;
    if(msg.args() > 0) {
      Tuple* tup = Tuple::create(state, msg.args());
      for(int i = msg.args() - 1; i >= 0; i--) {
        tup->put(state, i, msg.get_argument(i));
      }

      val = tup;
    } else {
      val = Qnil;
    }
    BlockContext* ctx = create_context(state, task->active());

    if(unlikely(task->profiler)) task->profiler->enter_block(state, home_, method_);

    // HACK: manually clear the stack used as args.
    task->active()->clear_stack(msg.stack);

    task->make_active(ctx);
    task->push(val);
    ctx->run(ctx->vmm, task, ctx);
  }

  // TODO - Untested!!!!!!!!!!
  ExecuteStatus BlockEnvironment::call_prim(STATE, Executable* exec, Task* task, Message& msg) {
    call(state, task, msg);
    return cExecuteRestart;
  }

  /*
   * Allocates a context, adjusting the initial stack pointer by the number of
   * locals the method requires.
   */
  BlockContext* BlockEnvironment::create_context(STATE, MethodContext* sender) {
    BlockContext* ctx = BlockContext::create(state, method_->stack_size()->to_native());
    ctx->sender(state, sender);
    ctx->block(state, this);
    ctx->cm(state, method_);
    ctx->home(state, home_);

    ctx->vmm = vmm ? vmm : method_->backend_method_;
    ctx->run = ctx->vmm->run;

    ctx->ip = 0;
    // HACK dup'd from MethodContext
    ctx->position_stack(method_->number_of_locals() - 1);

    return ctx;
  }

  BlockEnvironment* BlockEnvironment::under_context(STATE, CompiledMethod* cm,
      MethodContext* parent, MethodContext* active, size_t index) {

    BlockEnvironment* be = state->new_object<BlockEnvironment>(G(blokenv));


    VMMethod* vmm;
    if((vmm = active->vmm->blocks[index]) == NULL) {
      vmm = new VMMethod(state, cm);
      if(active->vmm->type) {
        vmm->specialize(state, active->vmm->type);
      }
      active->vmm->blocks[index] = vmm;
    }

    be->home(state, parent);
    be->home_block(state, active);
    be->method(state, cm);
    be->local_count(state, cm->local_count());
    be->vmm = vmm;

    return be;
  }

  BlockEnvironment* BlockEnvironment::dup(STATE) {
    BlockEnvironment* be = state->new_object<BlockEnvironment>(G(blokenv));

    be->home(state, home_);
    be->home_block(state, home_block_);
    be->method(state, method_);
    be->local_count(state, local_count_);
    be->vmm = this->vmm;

    return be;
  }


  void BlockEnvironment::Info::show(STATE, Object* self, int level) {
    BlockEnvironment* be = as<BlockEnvironment>(self);

    class_header(state, self);
    indent_attribute(++level, "home"); be->home()->show(state, level);
    indent_attribute(level, "home_block"); be->home_block()->show(state, level);
    indent_attribute(level, "local_count"); be->local_count()->show(state, level);
    indent_attribute(level, "method"); be->method()->show(state, level);
    close_body(level);
  }
}
