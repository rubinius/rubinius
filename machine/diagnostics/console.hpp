#ifndef RBX_DIAGNOSTICS_CONSOLE_HPP
#define RBX_DIAGNOSTICS_CONSOLE_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class ConsoleMetrics {
    public:
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
