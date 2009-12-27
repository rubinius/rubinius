#include "builtin/exception.hpp"
#include "thread_state.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {
  Object* ThreadState::as_object(STATE) {
    switch(raise_reason_) {
    case cException:
      return raise_value_.get();
    case cNone:
      return Qnil;
    default:
      Exception* exc = Exception::create(state);
      exc->klass(state, G(exc_vm_internal));
      exc->set_ivar(state, state->symbol("reason"), Fixnum::from(raise_reason_));
      exc->set_ivar(state, state->symbol("value"),  raise_value_.get());
      exc->set_ivar(state, state->symbol("destination"), destination_scope_.get());
      exc->set_ivar(state, state->symbol("throw_dest"), throw_dest_.get());
      return exc;
    }
  }

  void ThreadState::clear_exception(bool all) {
    if(raise_reason_ == cNone) return;
    if(!all && raise_reason_ != cException) {
      std::cout << "WARNING: clearing non exception raise reason!\n";
    }
    raise_value_.set(Qnil);
    raise_reason_ = cNone;
    destination_scope_.set(Qnil);
    throw_dest_.set(Qnil);
  }

  void ThreadState::set_exception(STATE, Object* obj) {
    if(obj->kind_of_p(state, G(exc_vm_internal))) {
      Object* reason = obj->get_ivar(state, state->symbol("reason"));
      raise_reason_ = (RaiseReason)as<Fixnum>(reason)->to_native();
      raise_value_.set(obj->get_ivar(state, state->symbol("value")));
      Object* vs = try_as<VariableScope>(
          obj->get_ivar(state, state->symbol("destination")));
      destination_scope_.set(vs ? vs : Qnil);
      throw_dest_.set(obj->get_ivar(state, state->symbol("throw_dest")));
    } else {
      raise_reason_ = cException;
      raise_value_.set(obj);
      destination_scope_.set(Qnil);
      throw_dest_.set(Qnil);
    }
  }

  void ThreadState::raise_exception(Exception* exc) {
    raise_reason_ = cException;
    raise_value_.set(exc);
    destination_scope_.set(Qnil);
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
    destination_scope_.set(Qnil);
  }

  void ThreadState::raise_throw(Symbol* dest, Object* value) {
    raise_reason_ = cCatchThrow;
    raise_value_.set(value);
    throw_dest_.set(dest);
  }
}
