#include "instructions.hpp"

#include "class/proc.hpp"

namespace rubinius {
  namespace instructions {
    inline bool push_proc(STATE, CallFrame* call_frame) {
      if(!call_frame->arguments) {
        Exception::internal_error(state, "no arguments object");
        return false;
      }

      Object* obj = call_frame->arguments->block();
      if(CBOOL(obj)) {
        Proc* prc = Proc::from_env(state, G(proc), obj);
        if(!prc) {
          Exception::internal_error(state, "invalid block type");
          return false;
        }
        stack_push(prc);
      } else {
        stack_push(cNil);
      }

      return true;
    }
  }
}
