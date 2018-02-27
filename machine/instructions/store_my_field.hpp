#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool store_my_field(STATE, CallFrame* call_frame, intptr_t index) {
      if(call_frame->self()->frozen_p()) {
        Exception::frozen_error(state, call_frame->self());
        return false;
      }
      call_frame->self()->set_field(state, index, stack_top());
      return true;
    }
  }
}
