/* A BlockEnvironment is created when a block is created. Its primary
 * operation is call, which activates the code associated with the block. */

#include "builtin/object.hpp"
#include "builtin/symbol.hpp"

#include "builtin/block_environment.hpp"
#include "objectmemory.hpp"

#include "vm/object_utils.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"
#include "builtin/system.hpp"
#include "builtin/staticscope.hpp"

#include "instruments/profiler.hpp"

#include <iostream>

namespace rubinius {

  void BlockEnvironment::init(STATE) {
    GO(blokenv).set(state->new_class("BlockEnvironment", G(object), G(rubinius)));
    G(blokenv)->set_object_type(state, BlockEnvironmentType);
    G(blokenv)->name(state, state->symbol("Rubinius::BlockEnvironment"));
  }

  BlockEnvironment* BlockEnvironment::allocate(STATE) {
    BlockEnvironment* env = state->new_object<BlockEnvironment>(G(blokenv));
    env->vmm = NULL;
    env->execute = &BlockEnvironment::execute_interpreter;
    return env;
  }

  // Installed by default in BlockEnvironment::execute, it runs the bytecodes
  // for the block in the interpreter.
  //
  // Future code will detect hot blocks and queue them in the JIT, whereby the
  // JIT will install a newly minted machine function into ::execute.
  Object* BlockEnvironment::execute_interpreter(STATE, CallFrame* previous,
                            BlockEnvironment* const env, Arguments& args,
                            BlockInvocation& invocation)
  {
    if(!env->vmm) {
      env->method_->formalize(state, false);
      env->vmm = env->method_->backend_method_;
    }

    VMMethod* const vmm = env->vmm;

    VariableScope* scope = (VariableScope*)alloca(sizeof(VariableScope) +
                               (vmm->number_of_locals * sizeof(Object*)));

    scope->setup_as_block(env->top_scope_, env->scope_,
                          env->method_, vmm->number_of_locals,
                          invocation.self);

    InterpreterCallFrame frame;
    frame.stk = (Object**)alloca(vmm->stack_size * sizeof(Object*));

    frame.prepare(vmm->stack_size);

    frame.previous = previous;
    frame.static_scope_ = invocation.static_scope;

    frame.msg =      NULL;
    frame.cm =       env->method_;
    frame.args =     args.total();
    frame.scope =    scope;
    frame.top_scope_ = env->top_scope_;
    frame.flags =    invocation.flags | CallFrame::cCustomStaticScope
                     | CallFrame::cMultipleScopes;

    Object* ret;

#ifdef RBX_PROFILER
    if(unlikely(state->shared.profiling())) {
      profiler::MethodEntry method(state,
          env->top_scope_->method()->name(), scope->module(), env->method_);
      ret = VMMethod::run_interpreter(state, vmm, &frame, args);
    } else {
      ret = VMMethod::run_interpreter(state, vmm, &frame, args);
    }
#else
    ret = VMMethod::run_interpreter(state, vmm, &frame, args);
#endif

    frame.scope->exit();

    return ret;
  }

  Object* BlockEnvironment::call(STATE, CallFrame* call_frame, Arguments& args, int flags) {
    BlockInvocation invocation(scope_->self(), method_->scope(), flags);
    return (*execute)(state, call_frame, this, args, invocation);
  }

  Object* BlockEnvironment::call_prim(STATE, Executable* exec,
      CallFrame* call_frame, Dispatch& msg, Arguments& args)
  {
    return call(state, call_frame, args);
  }

  Object* BlockEnvironment::call_on_object(STATE, CallFrame* call_frame,
                                           Arguments& args, int flags)
  {
    if(args.total() < 1) {
      Exception* exc =
        Exception::make_argument_error(state, 1, args.total(), state->symbol("__block__"));
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    Object* recv = args.shift(state);

    BlockInvocation invocation(recv, method_->scope(), flags);
    return (*execute)(state, call_frame, this, args, invocation);
  }

  Object* BlockEnvironment::call_under(STATE, Executable* exec,
      CallFrame* call_frame, Dispatch& msg, Arguments& args)
  {
    if(args.total() < 2) {
      Exception* exc =
        Exception::make_argument_error(state, 2, args.total(), state->symbol("__block__"));
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    Object* recv = args.shift(state);
    StaticScope* static_scope = as<StaticScope>(args.shift(state));

    BlockInvocation invocation(recv, static_scope, 0);
    return (*execute)(state, call_frame, this, args, invocation);
  }


  BlockEnvironment* BlockEnvironment::under_call_frame(STATE, CompiledMethod* cm,
      VMMethod* caller, CallFrame* call_frame, size_t index)
  {
    BlockEnvironment* be = state->new_object<BlockEnvironment>(G(blokenv));
    be->execute = &BlockEnvironment::execute_interpreter;

    VMMethod* vmm;
    if((vmm = caller->blocks[index]) == NULL) {
      vmm = new VMMethod(state, cm);
      if(caller->type) {
        vmm->specialize(state, caller->type);
      }
      caller->blocks[index] = vmm;
    }

    be->scope(state, call_frame->scope);
    be->top_scope(state, call_frame->top_scope());
    be->method(state, cm);
    be->local_count(state, cm->local_count());
    be->vmm = vmm;

    return be;
  }

  BlockEnvironment* BlockEnvironment::dup(STATE) {
    BlockEnvironment* be = state->new_object<BlockEnvironment>(G(blokenv));
    be->execute = &BlockEnvironment::execute_interpreter;

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
