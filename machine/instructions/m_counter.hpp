#include "instructions.hpp"

#include "diagnostics/measurement.hpp"

namespace rubinius {
  using namespace diagnostics;

  namespace instructions {
    inline void m_counter(STATE, CallFrame* call_frame, intptr_t value) {
      Measurement* m = reinterpret_cast<Measurement*>(value);

      m->update(state, 1);
    }
  }
}
