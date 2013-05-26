#ifndef RBX_THREAD_STATE_HPP
#define RBX_THREAD_STATE_HPP

#include "raise_reason.hpp"
#include "gc/root.hpp"
#include <iostream>

namespace rubinius {
  class Object;
  class Exception;
  class VariableScope;
  class VM;
  class ThreadState;

  class VMThreadState {
    TypedRoot<Exception*> current_exception_;
    TypedRoot<Object*> raise_value_;
    TypedRoot<Object*> throw_dest_;
    RaiseReason raise_reason_;
    TypedRoot<VariableScope*> destination_scope_;

  public:
    VMThreadState(VM* state);

    Exception* current_exception() {
      return current_exception_.get();
    }

    void set_current_exception(Exception* exc) {
      current_exception_.set(reinterpret_cast<Object*>(exc));
    }

    Object* raise_value() const {
      return raise_value_.get();
    }

    RaiseReason raise_reason() const {
      return raise_reason_;
    }

    VariableScope* destination_scope() const {
      return destination_scope_.get();
    }

    Object* throw_dest() const {
      return throw_dest_.get();
    }

    void clear_break();
    void clear_return();
    void clear_raise();
    void clear();

    ThreadState* state_as_object(STATE);
    void set_state(STATE, ThreadState* obj);

    void raise_exception(Exception* exc);
    void raise_return(Object* value, VariableScope* dest);
    void raise_break(Object* value, VariableScope* dest);
    void raise_exit(Object* code);
    void raise_throw(Object* dest, Object* value);
    void raise_thread_kill();
  };
};

#endif
