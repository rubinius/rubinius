#ifndef RBX_DIAGNOSTICS_CONSOLE_HPP
#define RBX_DIAGNOSTICS_CONSOLE_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class ConsoleMetrics : public Diagnostic {
    public:
      metric requests_received;
      metric responses_sent;

      ConsoleMetrics()
        : Diagnostic()
        , requests_received(0)
        , responses_sent(0)
      {
        set_type("ConsoleMetrics");
      }

      virtual void start_reporting(STATE) {
        if(state->shared().config.diagnostics_console_enabled) {
          Diagnostic::start_reporting(state);
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->shared().config.diagnostics_console_enabled) {
          Diagnostic::stop_reporting(state);
        }
      }
    };
  }
}

#endif
