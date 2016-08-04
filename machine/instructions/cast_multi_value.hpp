#include "interpreter/instructions.hpp"

inline bool rubinius::instruction_cast_multi_value(STATE, CallFrame* call_frame) {
  Object* value = stack_top();

  if(!kind_of<Array>(value)) {
    Object* res = Array::to_ary(state, value);
    if(!res) {
      return false;
    } else {
      stack_set_top(res);
    }
  }

  return true;
}
