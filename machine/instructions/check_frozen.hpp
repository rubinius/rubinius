#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool check_frozen(STATE, CallFrame* call_frame) {
      Object* value = stack_top();

      if(value->reference_p() && value->is_frozen_p()) {
        Exception::frozen_error(state, value);
        return false;
      }

      return true;
    }
  }
}
