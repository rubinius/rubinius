#ifndef RBX_DIAGNOSTICS_CODEDB_HPP
#define RBX_DIAGNOSTICS_CODEDB_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class CodeDBMetrics : public Diagnostic {
    public:
      metric load_ns;
      metric load_count;

      CodeDBMetrics()
        : Diagnostic()
        , load_ns(0)
        , load_count(0)
      {
        set_type("CodeDBMetrics");
      }

      virtual void start_reporting(STATE) {
        if(state->shared().config.diagnostics_codedb_enabled) {
          Diagnostic::start_reporting(state);
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->shared().config.diagnostics_codedb_enabled) {
          Diagnostic::stop_reporting(state);
        }
      }
    };
  }
}

#endif
