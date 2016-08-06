#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool set_ivar(STATE, CallFrame* call_frame, intptr_t literal) {
      if(CBOOL(call_frame->self()->frozen_p(state))) {
        Exception::frozen_error(state, call_frame->self());
        return false;
      }

      Symbol* sym = reinterpret_cast<Symbol*>(literal);
      call_frame->self()->set_ivar(state, sym, stack_top());
      return true;
    }
  }
}
