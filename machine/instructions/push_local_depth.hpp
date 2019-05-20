#include "instructions.hpp"

#include "class/variable_scope.hpp"

namespace rubinius {
  namespace instructions {
    inline bool push_local_depth(STATE, CallFrame* call_frame, intptr_t depth, intptr_t index) {
      if(depth == 0) {
        Exception::internal_error(state, "illegal push_local_depth usage");
        return false;
      } else {
        VariableScope* scope = call_frame->scope->parent();

        if(!scope || scope->nil_p()) {
          Exception::internal_error(state, "illegal push_local_depth usage, no parent");
          return false;
        }

        for(int j = 1; j < depth; j++) {
          scope = scope->parent();
          if(!scope || scope->nil_p()) {
            Exception::internal_error(state, "illegal push_local_depth usage, no parent");
            return false;
          }
        }

        if(index >= scope->number_of_locals()) {
          Exception::internal_error(state, "illegal push_local_depth usage, bad index");
          return false;
        }

        stack_push(scope->get_local(state, index));
        return true;
      }
    }
  }
}
