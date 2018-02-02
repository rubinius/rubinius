#ifndef RBX_DIAGNOSTICS_CONCURRENCY_HPP
#define RBX_DIAGNOSTICS_CONCURRENCY_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    struct LockMetrics {
      metric stop_the_world_ns;

      LockMetrics() {
        stop_the_world_ns = 0;
      }
    };
  }
}

#endif
