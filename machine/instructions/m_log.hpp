#include "instructions.hpp"
#include "logger.hpp"

namespace rubinius {
  namespace instructions {
    inline void m_log(STATE, CF, R0) {
      logger::write("instrument: %s: %s (+%d): %s",
          call_frame->compiled_code->file()->cpp_str(state).c_str(),
          call_frame->compiled_code->name()->cpp_str(state).c_str(),
          call_frame->ip(),
          as<Object>(RVAL(r0))->to_s(state)->c_str(state));
    }
  }
}
