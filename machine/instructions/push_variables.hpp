#include "interpreter/instructions.hpp"

#include "builtin/variable_scope.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_variables(STATE, CallFrame* call_frame) {
      stack_push(call_frame->promote_scope(state));
    }
  }
}
