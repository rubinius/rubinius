#include "builtin/exception.hpp"
#include "vm_thread_state.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"
#include "builtin/exception.hpp"
#include "builtin/thread_state.hpp"

namespace rubinius {
  VMThreadState::VMThreadState(VM* state)
    : current_exception_(state, nil<Exception>())
    , raise_value_(state, cNil)
    , throw_dest_(state, cNil)
    , raise_reason_(cNone)
    , destination_scope_(state, nil<VariableScope>())
  {}

  ThreadState* VMThreadState::state_as_object(STATE) {
    if(raise_reason_ == cNone && current_exception_.get()->nil_p()) return nil<ThreadState>();

    ThreadState* thread_state = ThreadState::create(state);
    thread_state->raise_reason(state, Fixnum::from(raise_reason_));
    thread_state->destination_scope(state, destination_scope());
    thread_state->throw_dest(state, throw_dest());
    thread_state->current_exception(state, current_exception());
    thread_state->raise_value(state, raise_value());
    return thread_state;
  }

  void VMThreadState::set_state(STATE, ThreadState* thread_state) {
    raise_reason_ = (RaiseReason)thread_state->raise_reason()->to_native();
    current_exception_.set(thread_state->current_exception());
    raise_value_.set(thread_state->raise_value());
    destination_scope_.set(thread_state->destination_scope());
    throw_dest_.set(thread_state->throw_dest());
  }

  void VMThreadState::clear() {
    raise_value_.set(cNil);
    raise_reason_ = cNone;
    destination_scope_.set(cNil);
    throw_dest_.set(cNil);
    current_exception_.set(cNil);
  }

  void VMThreadState::clear_break() {
    raise_reason_ = cNone;
    raise_value_.set(cNil);
    destination_scope_.set(cNil);
    throw_dest_.set(cNil);
  }

  void VMThreadState::clear_return() {
    // Use the same logic as break
    clear_break();
  }

  void VMThreadState::clear_raise() {
    clear_return();
    current_exception_.set(cNil);
  }

  void VMThreadState::raise_exception(Exception* exc) {
    raise_reason_ = cException;
    current_exception_.set(exc);
    destination_scope_.set(cNil);
  }

  void VMThreadState::raise_return(Object* value, VariableScope* dest) {
    raise_reason_ = cReturn;
    raise_value_.set(value);
    destination_scope_.set(dest);
  }

  void VMThreadState::raise_break(Object* value, VariableScope* dest) {
    raise_reason_ = cBreak;
    raise_value_.set(value);
    destination_scope_.set(dest);
  }

  void VMThreadState::raise_exit(Object* code) {
    raise_reason_ = cExit;
    raise_value_.set(code);
    destination_scope_.set(cNil);
  }

  void VMThreadState::raise_throw(Object* dest, Object* value) {
    raise_reason_ = cCatchThrow;
    raise_value_.set(value);
    throw_dest_.set(dest);
    current_exception_.set(cNil);
  }

  void VMThreadState::raise_thread_kill() {
    raise_reason_ = cThreadKill;
  }
}
