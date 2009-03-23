#include "builtin/exception.hpp"
#include "thread_state.hpp"
#include "builtin/variable_scope.hpp"

namespace rubinius {
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
}
