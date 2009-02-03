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

  Object* BlockEnvironment::call(STATE, Task* task, CallFrame* call_frame, size_t args) {
    Object* val;
    if(args > 0) {
      Tuple* tup = Tuple::create(state, args);
      int sp = 0;
      for(int i = args - 1; i >= 0; i--) {
        tup->put(state, i, call_frame->stack_back(sp++));
      }

      val = tup;
    } else {
      val = Qnil;
    }

    VariableScope* scope = (VariableScope*)alloca(sizeof(VariableScope) +
                               (vmm->number_of_locals * sizeof(Object*)));

    scope->setup_as_block(top_scope_, scope_, vmm->number_of_locals);

    CallFrame* cf = (CallFrame*)alloca(sizeof(CallFrame) + (vmm->stack_size * sizeof(Object*)));
    cf->setup(vmm->stack_size);

    cf->is_block = true;
    cf->previous = call_frame;
    cf->name =     (Symbol*)Qnil;
    cf->cm =       method_;
    cf->args =     args;
    cf->scope =    scope;
    cf->top_scope = top_scope_;
    cf->run =      vmm->run;
    cf->vmm =      vmm;

    // if(unlikely(task->profiler)) task->profiler->enter_block(state, home_, method_);

    cf->push(val);
    return cf->run(vmm, task, cf);
  }

  Object* BlockEnvironment::call(STATE, Task* task, CallFrame* call_frame, Message& msg) {
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

    VariableScope* scope = (VariableScope*)alloca(sizeof(VariableScope) +
                               (vmm->number_of_locals * sizeof(Object*)));

    scope->setup_as_block(top_scope_, scope_, vmm->number_of_locals);

    CallFrame* cf = (CallFrame*)alloca(sizeof(CallFrame) + (vmm->stack_size * sizeof(Object*)));
    cf->setup(vmm->stack_size);

    cf->is_block = true;
    cf->previous = call_frame;
    cf->name =     (Symbol*)Qnil;
    cf->cm =       method_;
    cf->args =     msg.args();
    cf->scope =    scope;
    cf->top_scope = top_scope_;
    cf->run =      vmm->run;
    cf->vmm =      vmm;

    // if(unlikely(task->profiler)) task->profiler->enter_block(state, home_, method_);

    cf->push(val);
    return cf->run(vmm, task, cf);
  }

  Object* BlockEnvironment::call_prim(STATE, Executable* exec, CallFrame* call_frame, Task* task, Message& msg) {
    return call(state, task, call_frame, msg);
  }

  BlockEnvironment* BlockEnvironment::under_call_frame(STATE, CompiledMethod* cm,
      CallFrame* call_frame, size_t index) {

    BlockEnvironment* be = state->new_object<BlockEnvironment>(G(blokenv));

    VMMethod* vmm;
    if((vmm = call_frame->vmm->blocks[index]) == NULL) {
      vmm = new VMMethod(state, cm);
      if(call_frame->vmm->type) {
        vmm->specialize(state, call_frame->vmm->type);
      }
      call_frame->vmm->blocks[index] = vmm;
    }

    be->scope(state, call_frame->scope);
    be->top_scope(state, call_frame->top_scope);
    be->method(state, cm);
    be->local_count(state, cm->local_count());
    be->vmm = vmm;

    return be;
  }

  BlockEnvironment* BlockEnvironment::dup(STATE) {
    BlockEnvironment* be = state->new_object<BlockEnvironment>(G(blokenv));

    be->scope(state, scope_);
    be->top_scope(state, top_scope_);
    be->method(state, method_);
    be->local_count(state, local_count_);
    be->vmm = this->vmm;

    return be;
  }


  void BlockEnvironment::Info::show(STATE, Object* self, int level) {
    BlockEnvironment* be = as<BlockEnvironment>(self);

    class_header(state, self);
    //indent_attribute(++level, "scope"); be->scope()->show(state, level);
    // indent_attribute(level, "top_scope"); be->top_scope()->show(state, level);
    indent_attribute(level, "local_count"); be->local_count()->show(state, level);
    indent_attribute(level, "method"); be->method()->show(state, level);
    close_body(level);
  }
}
