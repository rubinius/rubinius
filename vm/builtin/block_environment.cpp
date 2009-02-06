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

  Object* BlockEnvironment::call(STATE, CallFrame* call_frame, size_t args) {
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
    cf->name =     name_;
    cf->cm =       method_;
    cf->args =     args;
    cf->scope =    scope;
    cf->top_scope = top_scope_;

    // if(unlikely(task->profiler)) task->profiler->enter_block(state, home_, method_);

    cf->push(val);
    return vmm->run(state, vmm, cf);
  }

  Object* BlockEnvironment::call(STATE, CallFrame* call_frame, Message& msg) {
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
    cf->name =     name_;
    cf->cm =       method_;
    cf->args =     msg.args();
    cf->scope =    scope;
    cf->top_scope = top_scope_;

    // if(unlikely(task->profiler)) task->profiler->enter_block(state, home_, method_);

    cf->push(val);
    return vmm->run(state, vmm, cf);
  }

  Object* BlockEnvironment::call_prim(STATE, Executable* exec, CallFrame* call_frame, Message& msg) {
    return call(state, call_frame, msg);
  }

  BlockEnvironment* BlockEnvironment::under_call_frame(STATE, CompiledMethod* cm,
      VMMethod* caller, CallFrame* call_frame, size_t index) {

    BlockEnvironment* be = state->new_object<BlockEnvironment>(G(blokenv));

    VMMethod* vmm;
    if((vmm = caller->blocks[index]) == NULL) {
      vmm = new VMMethod(state, cm);
      if(caller->type) {
        vmm->specialize(state, caller->type);
      }
      caller->blocks[index] = vmm;
    }

    be->scope(state, call_frame->scope);
    be->top_scope(state, call_frame->top_scope);
    be->method(state, cm);
    be->local_count(state, cm->local_count());
    be->name(state, call_frame->name);
    be->vmm = vmm;

    return be;
  }

  BlockEnvironment* BlockEnvironment::dup(STATE) {
    BlockEnvironment* be = state->new_object<BlockEnvironment>(G(blokenv));

    be->scope(state, scope_);
    be->top_scope(state, top_scope_);
    be->method(state, method_);
    be->local_count(state, local_count_);
    be->name(state, name_);
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
