#include "instructions.hpp"

#include "class/compiled_code.hpp"
#include "class/string.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_file(STATE, CallFrame* call_frame) {
      String* str = String::create(state,
          call_frame->compiled_code->file()->cpp_str(state).c_str());

      stack_push(str);
    }
  }
}

