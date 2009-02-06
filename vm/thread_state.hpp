#ifndef RBX_THREAD_STATE_HPP
#define RBX_THREAD_STATE_HPP

#include "raise_reason.hpp"

namespace rubinius {
  class Object;
  class Exception;
  class VariableScope;

  class ThreadState {
    Object* raise_value_;
    RaiseReason raise_reason_;
    VariableScope* destination_scope_;

  public:
    ThreadState()
      : raise_value_(Qnil)
      , raise_reason_(cNone)
      , destination_scope_(NULL)
    {}

    Object* raise_value() {
      return raise_value_;
    }

    RaiseReason raise_reason() {
      return raise_reason_;
    }

    VariableScope* destination_scope() {
      return destination_scope_;
    }

    void clear_exception() {
      raise_value_ = Qnil;
      raise_reason_ = cNone;
      destination_scope_ = NULL;
    }

    void set_exception(Object* obj) {
      raise_value_ = obj;
      raise_reason_ = cException;
      destination_scope_ = NULL;
    }

    void raise_exception(Exception* exc);
    void raise_return(Object* value, VariableScope* dest);
    void raise_break(Object* value, VariableScope* dest);
    void raise_exit(Object* code);
  };
};

#endif
