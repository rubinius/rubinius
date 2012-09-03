#include "builtin/exception.hpp"
#include "thread_state.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"
#include "builtin/exception.hpp"

namespace rubinius {
  ThreadState::ThreadState(VM* state)
    : current_exception_(state, nil<Exception>())
    , raise_value_(state, cNil)
    , throw_dest_(state, cNil)
    , raise_reason_(cNone)
    , destination_scope_(state, nil<VariableScope>())
  {}

  Object* ThreadState::state_as_object(STATE) {
    if(raise_reason_ == cNone && current_exception_.get()->nil_p()) return cNil;

    Exception* exc = Exception::create(state);
    exc->klass(state, G(exc_vm_internal));
    exc->set_ivar(state, state->symbol("reason"), Fixnum::from(raise_reason_));
    exc->set_ivar(state, state->symbol("destination"), destination_scope());
    exc->set_ivar(state, state->symbol("throw_dest"), throw_dest());

    exc->set_ivar(state, state->symbol("exception"),  current_exception());
    exc->set_ivar(state, state->symbol("value"),  raise_value());
    return exc;
  }

  void ThreadState::set_state(STATE, Object* obj) {
    if(!obj->kind_of_p(state, G(exc_vm_internal))) return;

    Object* reason = obj->get_ivar(state, state->symbol("reason"));
    raise_reason_ = (RaiseReason)as<Fixnum>(reason)->to_native();

    current_exception_.set(obj->get_ivar(state, state->symbol("exception")));
    raise_value_.set(obj->get_ivar(state, state->symbol("value")));

    Object* vs = try_as<VariableScope>(
        obj->get_ivar(state, state->symbol("destination")));
    destination_scope_.set(vs ? vs : cNil);
    throw_dest_.set(obj->get_ivar(state, state->symbol("throw_dest")));
  }

  void ThreadState::clear() {
    raise_value_.set(cNil);
    raise_reason_ = cNone;
    destination_scope_.set(cNil);
    throw_dest_.set(cNil);
    current_exception_.set(cNil);
  }

  void ThreadState::clear_break() {
    raise_reason_ = cNone;
    raise_value_.set(cNil);
    destination_scope_.set(cNil);
    throw_dest_.set(cNil);
  }

  void ThreadState::clear_return() {
    // Use the same logic as break
    clear_break();
  }

  void ThreadState::clear_raise() {
    clear_return();
    current_exception_.set(cNil);
  }

  void ThreadState::raise_exception(Exception* exc) {
    raise_reason_ = cException;
    current_exception_.set(exc);
    destination_scope_.set(cNil);
  }

  void ThreadState::raise_return(Object* value, VariableScope* dest) {
    raise_reason_ = cReturn;
    raise_value_.set(value);
    destination_scope_.set(dest);
  }

  void ThreadState::raise_break(Object* value, VariableScope* dest) {
    raise_reason_ = cBreak;
    raise_value_.set(value);
    destination_scope_.set(dest);
  }

  void ThreadState::raise_exit(Object* code) {
    raise_reason_ = cExit;
    raise_value_.set(code);
    destination_scope_.set(cNil);
  }

  void ThreadState::raise_throw(Object* dest, Object* value) {
    raise_reason_ = cCatchThrow;
    raise_value_.set(value);
    throw_dest_.set(dest);
  }

  void ThreadState::raise_thread_kill() {
    raise_reason_ = cThreadKill;
  }
}
