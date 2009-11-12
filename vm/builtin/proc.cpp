#include "prelude.hpp"
#include "builtin/proc.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "builtin/class.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/system.hpp"
#include "arguments.hpp"

#include "vmmethod.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"

namespace rubinius {

  void Proc::init(STATE) {
    GO(proc).set(
        state->new_class("Proc", G(object)));
    G(proc)->set_object_type(state, ProcType);
  }

  Proc* Proc::create(STATE, Object* self) {
    return state->new_object<Proc>(as<Class>(self));
  }

  Proc* Proc::from_env(STATE, Object* env) {
    if(Proc* p = try_as<Proc>(env)) {
      return p;
    }

    if(BlockEnvironment* be = try_as<BlockEnvironment>(env)) {
      Proc* proc = Proc::create(state, G(proc));
      proc->block(state, be);
      return proc;
    }

    return reinterpret_cast<Proc*>(Primitives::failure());
  }

  Object* Proc::call(STATE, CallFrame* call_frame, Arguments& args) {
    bool lambda_style = !lambda_->nil_p();
    int flags = 0;

    // Check the arity in lambda mode
    if(bound_method_->nil_p() && lambda_style) {
      flags = CallFrame::cIsLambda;
      int required = block_->method()->required_args()->to_native();

      if(required >= 0 && (size_t)required != args.total()) {
        Exception* exc =
          Exception::make_argument_error(state, required, args.total(),
              state->symbol("__block__"));
        exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
        state->thread_state()->raise_exception(exc);
        return NULL;
      }
    }

    Object* ret;
    if(bound_method_->nil_p()) {
      ret= block_->call(state, call_frame, args, flags);
    } else {
      Dispatch dis(G(sym_call));
      ret = dis.send(state, call_frame, args);
    }

    return ret;
  }

  Object* Proc::yield(STATE, CallFrame* call_frame, Arguments& args) {
    if(bound_method_->nil_p()) {
      // NOTE! To match MRI semantics, this explicitely ignores lambda_.
      return block_->call(state, call_frame, args, 0);
    } else {
      Dispatch dis;
      return call_prim(state, NULL, call_frame, dis, args);
    }
  }

  Object* Proc::call_prim(STATE, Executable* exec, CallFrame* call_frame, Dispatch& msg,
                          Arguments& args) {
    bool lambda_style = !lambda_->nil_p();
    int flags = 0;

    // Check the arity in lambda mode
    if(lambda_style) {
      flags = CallFrame::cIsLambda;
      int required = block_->method()->required_args()->to_native();

      // Bug-to-bug compatibility: when required is 0 or 1, we accept any number of
      // args. Why? No fucking clue. So thats why we test for 2 here.
      if(required >= 2 && (size_t)required != args.total()) {
        Exception* exc =
          Exception::make_argument_error(state, required, args.total(), state->symbol("__block__"));
        exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
        state->thread_state()->raise_exception(exc);
        return NULL;
      }
    }

    Object* ret;
    if(bound_method_->nil_p()) {
      ret = block_->call(state, call_frame, args, flags);
    } else {
      Dispatch dis(G(sym_call));
      ret = dis.send(state, call_frame, args);
    }

    return ret;
  }

  Object* Proc::call_on_object(STATE, Executable* exec, CallFrame* call_frame,
                               Dispatch& msg, Arguments& args) {
    bool lambda_style = !lambda_->nil_p();
    int flags = 0;

    // Check the arity in lambda mode
    if(lambda_style) {
      flags = CallFrame::cIsLambda;
      int required = block_->method()->required_args()->to_native();

      if(args.total() < 1 || (required >= 0 && (size_t)required != args.total() - 1)) {
        Exception* exc =
          Exception::make_argument_error(state, required, args.total(), state->symbol("__block__"));
        exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
        state->thread_state()->raise_exception(exc);
        return NULL;
      }
    }

    // Since we allow Proc's to be created without setting block_, we need to check
    // it.
    if(block_->nil_p()) {
      Exception* exc =
        Exception::make_type_error(state, BlockEnvironment::type, block_, "Invalid proc style");
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    return block_->call_on_object(state, call_frame, args, flags);
  }
}
