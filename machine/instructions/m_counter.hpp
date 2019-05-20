#include "instructions.hpp"

#include "diagnostics/measurement.hpp"

namespace rubinius {
  using namespace diagnostics;

  namespace instructions {
    inline void m_counter(STATE, CallFrame* call_frame, intptr_t value) {
      Counter* counter = reinterpret_cast<Counter*>(value);

      counter->set(state, 1);
    }
  }
}
