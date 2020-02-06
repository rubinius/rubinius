#include "memory.hpp"

#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/unwind_state.hpp"

namespace rubinius {

  void UnwindState::bootstrap(STATE) {
    GO(unwind_state).set(state->memory()->new_class<Class, UnwindState>(
          state, G(rubinius), "UnwindState"));
  }

  UnwindState* UnwindState::create(STATE) {
    return state->memory()->new_object<UnwindState>(state, G(unwind_state));
  }

  UnwindState* UnwindState::copy(STATE) {
    UnwindState* copy = UnwindState::create(state);

    copy->current_exception(current_exception());
    copy->raise_value(raise_value());
    copy->throw_dest(throw_dest());
    copy->destination_scope(destination_scope());
    copy->raise_reason(raise_reason());

    return copy;
  }

  void UnwindState::set_state(UnwindState* unwind_state) {
    raise_reason(unwind_state->raise_reason());
    current_exception(unwind_state->current_exception());
    raise_value(unwind_state->raise_value());
    destination_scope(unwind_state->destination_scope());
    throw_dest(unwind_state->throw_dest());
  }

  void UnwindState::clear() {
    raise_value(cNil);
    raise_reason(cNone);
    destination_scope(nil<VariableScope>());
    throw_dest(cNil);
    current_exception(nil<Exception>());
  }

  void UnwindState::clear_break() {
    raise_reason(cNone);
    raise_value(cNil);
    destination_scope(nil<VariableScope>());
    throw_dest(cNil);
  }

  void UnwindState::clear_return() {
    // Use the same logic as break
    clear_break();
  }

  void UnwindState::clear_raise() {
    clear_return();
    current_exception(nil<Exception>());
  }

  void UnwindState::raise_exception(Exception* exc) {
    raise_reason(cException);
    current_exception(exc);
    destination_scope(nil<VariableScope>());
  }

  void UnwindState::raise_return(Object* value, VariableScope* dest) {
    raise_reason(cReturn);
    raise_value(value);
    destination_scope(dest);
  }

  void UnwindState::raise_break(Object* value, VariableScope* dest) {
    raise_reason(cBreak);
    raise_value(value);
    destination_scope(dest);
  }

  void UnwindState::raise_exit(Object* code) {
    raise_reason(cSystemExit);
    raise_value(code);
    destination_scope(nil<VariableScope>());
  }

  void UnwindState::raise_throw(Object* dest, Object* value) {
    raise_reason(cCatchThrow);
    raise_value(value);
    throw_dest(dest);
    current_exception(nil<Exception>());
  }

  void UnwindState::raise_thread_kill() {
    raise_reason(cThreadKill);
  }

  void UnwindState::raise_fiber_cancel() {
    raise_reason(cFiberCancel);
  }
}
