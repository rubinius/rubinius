#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline intptr_t ret(STATE, CallFrame* call_frame) {
      if(call_frame->scope->made_alias_p()) {
        call_frame->scope->flush_to_heap(state);
      }
      return (intptr_t)stack_top();
    }
  }
}
