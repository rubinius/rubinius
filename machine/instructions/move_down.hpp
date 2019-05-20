#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void move_down(CallFrame* call_frame, intptr_t positions) {
      Object* val = stack_top();
      for(int i = 0; i < positions; i++) {
        int target = -i;
        int current = target - 1;
        STACK_PTR[target] = STACK_PTR[current];
      }
      STACK_PTR[-positions] = val;
    }
  }
}
