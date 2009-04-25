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
    return env;
  }

  /** @todo Duplicates VMMethod functionality. Refactor. --rue */
  Object* BlockEnvironment::call(STATE, CallFrame* call_frame, size_t args, int flags) {
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

    if(!vmm) {
      method_->formalize(state, false);
      vmm = method_->backend_method_;
    }

    VariableScope* scope = (VariableScope*)alloca(sizeof(VariableScope) +
                               (vmm->number_of_locals * sizeof(Object*)));

    scope->setup_as_block(top_scope_, scope_, method_, vmm->number_of_locals);

    CallFrame* frame = ALLOCA_CALLFRAME(vmm);
    frame->prepare(vmm->stack_size);

    frame->is_block = true;
    frame->previous = call_frame;
    frame->static_scope = method_->scope();

    frame->name =     reinterpret_cast<Symbol*>(Qnil);
    frame->cm =       method_;
    frame->args =     args;
    frame->scope =    scope;
    frame->top_scope = top_scope_;
    frame->flags =    flags;

    frame->push(val);
    Object* ret;

#ifdef RBX_PROFILER
    if(unlikely(state->shared.profiling())) {
      profiler::MethodEntry method(state,
          top_scope_->method()->name(), scope->module(), method_);
      ret = VMMethod::run_interpreter(state, vmm, frame);
    } else {
      ret = VMMethod::run_interpreter(state, vmm, frame);
    }
#else
    ret = VMMethod::run_interpreter(state, vmm, frame);
#endif

    frame->scope->exit();

    return ret;
  }

  /** @todo See above. --rue */
  Object* BlockEnvironment::call(STATE, CallFrame* call_frame, Arguments& args, int flags) {
    Object* val;
    if(args.total() > 0) {
      Tuple* tup = Tuple::create(state, args.total());
      for(int i = args.total() - 1; i >= 0; i--) {
        tup->put(state, i, args.get_argument(i));
      }

      val = tup;
    } else {
      val = Qnil;
    }

    VariableScope* scope = (VariableScope*)alloca(sizeof(VariableScope) +
                               (vmm->number_of_locals * sizeof(Object*)));

    scope->setup_as_block(top_scope_, scope_, method_, vmm->number_of_locals);

    CallFrame* frame = ALLOCA_CALLFRAME(vmm);
    frame->prepare(vmm->stack_size);

    frame->is_block = true;
    frame->previous = call_frame;
    frame->static_scope = method_->scope();

    frame->name =     reinterpret_cast<Symbol*>(Qnil);
    frame->cm =       method_;
    frame->args =     args.total();
    frame->scope =    scope;
    frame->top_scope = top_scope_;
    frame->flags =    flags;

    frame->push(val);
    Object* ret;

#ifdef RBX_PROFILER
    if(unlikely(state->shared.profiling())) {
      profiler::MethodEntry method(state,
          top_scope_->method()->name(), scope->module(), method_);
      ret = VMMethod::run_interpreter(state, vmm, frame);
    } else {
      ret = VMMethod::run_interpreter(state, vmm, frame);
    }
#else
    ret = VMMethod::run_interpreter(state, vmm, frame);
#endif

    return ret;
  }

  Object* BlockEnvironment::call_prim(STATE, Executable* exec,
      CallFrame* call_frame, Dispatch& msg, Arguments& args) {
    return call(state, call_frame, args);
  }

  /** @todo See above. --emp */
  Object* BlockEnvironment::call_on_object(STATE, CallFrame* call_frame,
                                           Arguments& args, int flags) {
    Object* val;

    if(args.total() < 1) {
      Exception* exc =
        Exception::make_argument_error(state, 1, args.total(), state->symbol("__block__"));
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    Object* recv = args.get_argument(0);

    if(args.total() > 1) {
      Tuple* tup = Tuple::create(state, args.total() - 1);
      for(size_t i = 0, j = 1; j < args.total(); i++, j++) {
        tup->put(state, i, args.get_argument(j));
      }

      val = tup;
    } else {
      val = Qnil;
    }

    VariableScope* scope = (VariableScope*)alloca(sizeof(VariableScope) +
                               (vmm->number_of_locals * sizeof(Object*)));

    scope->setup_as_block(top_scope_, scope_, method_, vmm->number_of_locals, recv);

    CallFrame* frame = ALLOCA_CALLFRAME(vmm);
    frame->prepare(vmm->stack_size);

    frame->is_block = true;
    frame->previous = call_frame;
    frame->static_scope = method_->scope();

    frame->name =     reinterpret_cast<Symbol*>(Qnil);
    frame->cm =       method_;
    frame->args =     args.total();
    frame->scope =    scope;
    frame->top_scope = top_scope_;
    frame->flags =    flags;

    frame->push(val);
    Object* ret;

#ifdef RBX_PROFILER
    if(unlikely(state->shared.profiling())) {
      profiler::MethodEntry method(state,
          top_scope_->method()->name(), scope->module(), method_);
      ret = VMMethod::run_interpreter(state, vmm, frame);
    } else {
      ret = VMMethod::run_interpreter(state, vmm, frame);
    }
#else
    ret = VMMethod::run_interpreter(state, vmm, frame);
#endif

    return ret;
  }

  /** @todo See above. --emp */
  Object* BlockEnvironment::call_under(STATE, Executable* exec,
      CallFrame* call_frame, Dispatch& msg, Arguments& args) {

    Object* val;

    if(args.total() < 2) {
      Exception* exc =
        Exception::make_argument_error(state, 2, args.total(), state->symbol("__block__"));
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    Object* recv = args.get_argument(0);
    StaticScope* static_scope = as<StaticScope>(args.get_argument(1));

    if(args.total() > 2) {
      Tuple* tup = Tuple::create(state, args.total() - 2);
      for(size_t i = 0, j = 2; j < args.total(); i++, j++) {
        tup->put(state, i, args.get_argument(j));
      }

      val = tup;
    } else {
      val = Qnil;
    }

    if(!vmm) {
      method_->formalize(state, false);
      vmm = method_->backend_method_;
    }

    VariableScope* scope = (VariableScope*)alloca(sizeof(VariableScope) +
                               (vmm->number_of_locals * sizeof(Object*)));

    scope->setup_as_block(top_scope_, scope_, method_, vmm->number_of_locals, recv);

    CallFrame* frame = ALLOCA_CALLFRAME(vmm);
    frame->prepare(vmm->stack_size);

    frame->is_block = true;
    frame->previous = call_frame;
    frame->static_scope = static_scope;

    frame->name =     reinterpret_cast<Symbol*>(Qnil);
    frame->cm =       method_;
    frame->args =     args.total();
    frame->scope =    scope;
    frame->top_scope = top_scope_;
    frame->flags =    0;

    frame->push(val);
    Object* ret;

#ifdef RBX_PROFILER
    if(unlikely(state->shared.profiling())) {
      profiler::MethodEntry method(state,
          top_scope_->method()->name(), scope->module(), method_);
      ret = VMMethod::run_interpreter(state, vmm, frame);
    } else {
      ret = VMMethod::run_interpreter(state, vmm, frame);
    }
#else
    ret = VMMethod::run_interpreter(state, vmm, frame);
#endif

    return ret;
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
