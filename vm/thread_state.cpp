#include "builtin/exception.hpp"
#include "thread_state.hpp"

namespace rubinius {
  void ThreadState::raise_exception(Exception* exc) {
    raise_reason_ = cException;
    raise_value_ = exc;
    destination_scope_ = 0;
  }

  void ThreadState::raise_return(Object* value, VariableScope* dest) {
    raise_reason_ = cReturn;
    raise_value_ = value;
    destination_scope_ = dest;
  }

  void ThreadState::raise_break(Object* value, VariableScope* dest) {
    raise_reason_ = cBreak;
    raise_value_ = value;
    destination_scope_ = dest;
  }
}
