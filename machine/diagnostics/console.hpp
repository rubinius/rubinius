#ifndef RBX_DIAGNOSTICS_CONSOLE_HPP
#define RBX_DIAGNOSTICS_CONSOLE_HPP

#include "diagnostics/metrics.hpp"

namespace rubinius {
  namespace diagnostics {
    struct ConsoleMetrics {
      metric requests_received;
      metric responses_sent;

      ConsoleMetrics() {
        requests_received = 0;
        responses_sent = 0;
      }
    };
  }
}

#endif
