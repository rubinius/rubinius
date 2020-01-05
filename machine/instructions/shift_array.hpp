#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void shift_array(STATE, CallFrame* call_frame) {
      Array* array = as<Array>(stack_pop());
      intptr_t size = array->size();

      if(size == 0) {
        stack_push(array);
        stack_push(cNil);
      } else {
        intptr_t j = size - 1;
        Object* shifted_value = array->get(state, 0);
        // TODO: ensure cannot be NULL
        Array* smaller_array = Array::create(state, j);

        for(intptr_t i = 0; i < j; i++) {
          smaller_array->set(state, i, array->get(state, i+1));
        }

        stack_push(smaller_array);
        stack_push(shifted_value);
      }
    }
  }
}
