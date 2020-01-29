#include "class/exception.hpp"
#include "vm_thread_state.hpp"
#include "class/variable_scope.hpp"
#include "class/fixnum.hpp"
#include "class/class.hpp"
#include "class/symbol.hpp"
#include "class/exception.hpp"
#include "class/thread_state.hpp"

namespace rubinius {
  VMThreadState::VMThreadState(VM* state)
    : current_exception_(nil<Exception>())
    , raise_value_(cNil)
    , throw_dest_(cNil)
    , destination_scope_(nil<VariableScope>())
    , raise_reason_(cNone)
  {}

  ThreadState* VMThreadState::state_as_object(STATE) {
    if(raise_reason_ == cNone && current_exception_->nil_p()) return nil<ThreadState>();

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
    current_exception_ = thread_state->current_exception();
    raise_value_ = thread_state->raise_value();
    destination_scope_ = thread_state->destination_scope();
    throw_dest_ = thread_state->throw_dest();
  }

  void VMThreadState::set_state(VMThreadState* thread_state) {
    raise_reason_ = thread_state->raise_reason();
    current_exception_ = thread_state->current_exception();
    raise_value_ = thread_state->raise_value();
    destination_scope_ = thread_state->destination_scope();
    throw_dest_ = thread_state->throw_dest();
  }

  void VMThreadState::clear() {
    raise_value_ = cNil;
    raise_reason_ = cNone;
    destination_scope_ = nil<VariableScope>();
    throw_dest_ = cNil;
    current_exception_ = nil<Exception>();
  }

  void VMThreadState::clear_break() {
    raise_reason_ = cNone;
    raise_value_ = cNil;
    destination_scope_ = nil<VariableScope>();
    throw_dest_ = cNil;
  }

  void VMThreadState::clear_return() {
    // Use the same logic as break
    clear_break();
  }

  void VMThreadState::clear_raise() {
    clear_return();
    current_exception_ = nil<Exception>();
  }

  void VMThreadState::raise_exception(Exception* exc) {
    raise_reason_ = cException;
    current_exception_ = exc;
    destination_scope_ = nil<VariableScope>();
  }

  void VMThreadState::raise_return(Object* value, VariableScope* dest) {
    raise_reason_ = cReturn;
    raise_value_ = value;
    destination_scope_ = nil<VariableScope>();
  }

  void VMThreadState::raise_break(Object* value, VariableScope* dest) {
    raise_reason_ = cBreak;
    raise_value_ = value;
    destination_scope_ = nil<VariableScope>();
  }

  void VMThreadState::raise_exit(Object* code) {
    raise_reason_ = cExit;
    raise_value_ = code;
    destination_scope_ = nil<VariableScope>();
  }

  void VMThreadState::raise_throw(Object* dest, Object* value) {
    raise_reason_ = cCatchThrow;
    raise_value_ = value;
    throw_dest_ = dest;
    current_exception_ = nil<Exception>();
  }

  void VMThreadState::raise_thread_kill() {
    raise_reason_ = cThreadKill;
  }

  void VMThreadState::raise_fiber_cancel() {
    raise_reason_ = cFiberCancel;
  }

  void VMThreadState::gc_scan(STATE, std::function<void (STATE, Object**)> f) {
    f(state, reinterpret_cast<Object**>(&current_exception_));
    f(state, reinterpret_cast<Object**>(&raise_value_));
    f(state, reinterpret_cast<Object**>(&throw_dest_));
    f(state, reinterpret_cast<Object**>(&destination_scope_));
  }
}
