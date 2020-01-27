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

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_.AddMember("requests_received", requests_received, alloc);
        document_.AddMember("responses_sent", responses_sent, alloc);
      }

      virtual ~ConsoleMetrics() { }

      virtual void update() {
        document_["requests_received"] = requests_received;
        document_["responses_sent"] = responses_sent;
      }

      virtual void start_reporting(STATE) {
        if(state->configuration()->diagnostics_console_enabled) {
          Diagnostic::start_reporting(state);
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->configuration()->diagnostics_console_enabled) {
          Diagnostic::stop_reporting(state);
        }
      }
    };
  }
}

#endif
