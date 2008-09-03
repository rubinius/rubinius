/* A BlockEnvironment is created when a block is created. Its primary
 * operation is call, which activates the code associated with the block. */

#include "block_environment.hpp"
#include "objectmemory.hpp"

#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/contexts.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/tuple.hpp"

#include <iostream>

namespace rubinius {

  void BlockEnvironment::init(STATE) {
    GO(blokenv).set(state->new_class("BlockEnvironment", G(object), 
          BlockEnvironment::fields));
    G(blokenv)->set_object_type(BlockEnvType);
  }

  void BlockEnvironment::call(STATE, Task* task, size_t args) {
    OBJECT val;
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
    BlockContext* ctx = create_context(state, task->active);
    task->make_active(ctx);
    task->push(val);
  }

  void BlockEnvironment::call(STATE, Task* task, Message& msg) {
    OBJECT val;
    if(msg.args() > 0) {
      Tuple* tup = Tuple::create(state, msg.args());
      for(int i = msg.args() - 1; i >= 0; i--) {
        tup->put(state, i, msg.get_argument(i));
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

  // TODO - Untested!!!!!!!!!!
  bool BlockEnvironment::call_prim(STATE, VMExecutable* exec, Task* task, Message& msg) {
    call(state, task, msg);
    return true;
  }

  /*
   * Allocates a context, adjusting the initial stack pointer by the number of
   * locals the method requires.
   */
  BlockContext* BlockEnvironment::create_context(STATE, MethodContext* sender) {
    BlockContext* ctx = BlockContext::create(state, method->stack_size->to_native());
    SET(ctx, sender, sender);
    SET(ctx, block, this);
    SET(ctx, cm, method);
    SET(ctx, home, home);

    ctx->vmm = vmm;
    ctx->ip = 0;
    // HACK dup'd from MethodContext
    ctx->position_stack(method->number_of_locals() - 1);

    return ctx;
  }

  BlockEnvironment* BlockEnvironment::under_context(STATE, CompiledMethod* cm,
      MethodContext* parent, MethodContext* active, size_t index) {

    BlockEnvironment* be = (BlockEnvironment*)state->new_object(G(blokenv));


    VMMethod* vmm;
    if((vmm = active->vmm->blocks[index]) == NULL) {
      vmm = new VMMethod(state, cm);
      if(active->vmm->type) {
        vmm->specialize(active->vmm->type);
      }
      active->vmm->blocks[index] = vmm;
    }

    SET(be, home, parent);
    SET(be, home_block, active);
    SET(be, method, cm);
    SET(be, local_count, cm->local_count);
    be->vmm = vmm;

    return be;
  }

  void BlockEnvironment::Info::show(STATE, OBJECT self, int level) {
    BlockEnvironment* be = as<BlockEnvironment>(self);

    class_header(self);
    indent(++level); std::cout << "home: "; be->home->show(state, level);
    indent(level); std::cout << "home_block: "; be->home_block->show(state, level);
    indent(level); std::cout << "local_count: "; be->local_count->show(state, level);
    indent(level); std::cout << "method: "; be->method->show(state, level);
    indent(--level); std::cout << ">\n";
  }
}
