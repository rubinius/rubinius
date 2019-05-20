#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void rotate(CallFrame* call_frame, intptr_t count) {
      int diff = count >> 1;
      Object** start = STACK_PTR - (count - 1);
      Object** end = STACK_PTR;

      for(int i = 0; i < diff; i++) {
        Object* right = *end;
        Object* left =  *start;

        *start = right;
        *end = left;

        start++;
        end--;
      }
    }
  }
}
