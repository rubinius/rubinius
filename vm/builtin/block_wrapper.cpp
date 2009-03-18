#include "prelude.hpp"
#include "builtin/block_wrapper.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "builtin/class.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/system.hpp"

#include "vmmethod.hpp"
#include "message.hpp"

namespace rubinius {

  void BlockWrapper::init(STATE) {
    GO(block_wrapper).set(
        state->new_class("Proc", G(object)));
    G(block_wrapper)->set_object_type(state, BlockEnvironmentType);
  }

  BlockWrapper* BlockWrapper::create(STATE, Object* self) {
    return state->new_object<BlockWrapper>(as<Class>(self));
  }

  BlockWrapper* BlockWrapper::from_env(STATE, BlockEnvironment* env) {
    BlockWrapper* wrapper = BlockWrapper::create(state, G(block_wrapper));
    wrapper->block(state, env);
    return wrapper;
  }

  Object* BlockWrapper::call(STATE, CallFrame* call_frame, size_t args) {
    bool lambda_style = !lambda_->nil_p();
    int flags = 0;

    // Check the arity in lambda mode
    if(lambda_style) {
      flags = CallFrame::cIsLambda;
      int required = block_->method()->required_args()->to_native();

      if(required >= 0 && (size_t)required != args) {
        Exception* exc =
          Exception::make_argument_error(state, required, args, state->symbol("__block__"));
        exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
        state->thread_state()->raise_exception(exc);
        return NULL;
      }
    }

    Object* ret = block_->call(state, call_frame, args, flags);

    if(lambda_style && !ret) {
      RaiseReason reason = state->thread_state()->raise_reason();
      if(reason == cReturn || reason == cBreak) {
        // TODO investigate if we should check the destination_scope here.
        // It doesn't appear that MRI checks anything similar.
        ret = state->thread_state()->raise_value();
        state->thread_state()->clear_exception();
      }
    }

    return ret;
  }

  Object* BlockWrapper::yield(STATE, CallFrame* call_frame, size_t args) {
    // NOTE! To match MRI semantics, this explicitely ignores lambda_.
    return block_->call(state, call_frame, args, 0);
  }

  Object* BlockWrapper::yield(STATE, CallFrame* call_frame, Message& msg) {
    // NOTE! To match MRI semantics, this explicitely ignores lambda_.
    return block_->call(state, call_frame, msg, 0);
  }

  Object* BlockWrapper::call_prim(STATE, Executable* exec, CallFrame* call_frame, Message& msg) {
    bool lambda_style = !lambda_->nil_p();
    int flags = 0;

    // Check the arity in lambda mode
    if(lambda_style) {
      flags = CallFrame::cIsLambda;
      int required = block_->method()->required_args()->to_native();

      if(required >= 0 && (size_t)required != msg.args()) {
        Exception* exc =
          Exception::make_argument_error(state, required, msg.args(), state->symbol("__block__"));
        exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
        state->thread_state()->raise_exception(exc);
        return NULL;
      }
    }

    Object* ret = block_->call(state, call_frame, msg, flags);

    if(lambda_style && !ret) {
      RaiseReason reason = state->thread_state()->raise_reason();
      if(reason == cReturn || reason == cBreak) {
        // TODO investigate if we should check the destination_scope here.
        // It doesn't appear that MRI checks anything similar.
        ret = state->thread_state()->raise_value();
        state->thread_state()->clear_exception();
      }
    }

    return ret;
  }

  Object* BlockWrapper::call_on_object(STATE, Executable* exec, CallFrame* call_frame, Message& msg) {
    bool lambda_style = !lambda_->nil_p();
    int flags = 0;

    // Check the arity in lambda mode
    if(lambda_style) {
      flags = CallFrame::cIsLambda;
      int required = block_->method()->required_args()->to_native();

      if(msg.args() < 1 || (required >= 0 && (size_t)required != msg.args() - 1)) {
        Exception* exc =
          Exception::make_argument_error(state, required, msg.args(), state->symbol("__block__"));
        exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
        state->thread_state()->raise_exception(exc);
        return NULL;
      }
    }

    Object* ret = block_->call_on_object(state, call_frame, msg, flags);

    if(lambda_style && !ret) {
      RaiseReason reason = state->thread_state()->raise_reason();
      if(reason == cReturn || reason == cBreak) {
        // TODO investigate if we should check the destination_scope here.
        // It doesn't appear that MRI checks anything similar.
        ret = state->thread_state()->raise_value();
        state->thread_state()->clear_exception();
      }
    }

    return ret;
  }
}
