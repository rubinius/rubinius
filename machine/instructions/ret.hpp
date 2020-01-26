#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline intptr_t ret(STATE, CallFrame* call_frame) {
      if(call_frame->scope->made_alias_p()) {
        call_frame->scope->flush_to_heap(state);
      }

      Object* obj = stack_top();

      if(obj->nil_p()) {
        if(NIL_TAG_ID(obj) == 0) {
          obj = reinterpret_cast<Object*>(
              APPLY_NIL_TAG(call_frame->machine_code->nil_id(), call_frame->ip()));
        }
      }

      call_frame->return_value = obj;

      return reinterpret_cast<intptr_t>(obj);
    }
  }
}
