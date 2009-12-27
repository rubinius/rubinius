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

  class ThreadState {
    TypedRoot<Object*> raise_value_;
    TypedRoot<Object*> throw_dest_;
    RaiseReason raise_reason_;
    TypedRoot<VariableScope*> destination_scope_;

  public:
    ThreadState(VM* vm)
      : raise_value_(vm, Qnil)
      , throw_dest_(vm, Qnil)
      , raise_reason_(cNone)
      , destination_scope_(vm, (VariableScope*)Qnil)
    {}

    Object* raise_value() {
      return raise_value_.get();
    }

    RaiseReason raise_reason() {
      return raise_reason_;
    }

    VariableScope* destination_scope() {
      return destination_scope_.get();
    }

    Object* throw_dest() {
      return throw_dest_.get();
    }

    void clear_exception(bool all = false);

    Object* as_object(STATE);
    void set_exception(STATE, Object* obj);

    void raise_exception(Exception* exc);
    void raise_return(Object* value, VariableScope* dest);
    void raise_break(Object* value, VariableScope* dest);
    void raise_exit(Object* code);
    void raise_throw(Symbol* dest, Object* value);
  };
};

#endif
