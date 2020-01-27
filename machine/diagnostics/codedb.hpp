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

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_.AddMember("load.ns", load_ns, alloc);
        document_.AddMember("load_count", load_count, alloc);
      }

      virtual ~CodeDBMetrics() { }

      virtual void update() {
        document_["load.ns"] = load_ns;
        document_["load_count"] = load_count;
      }

      virtual void start_reporting(STATE) {
        if(state->configuration()->diagnostics_codedb_enabled) {
          Diagnostic::start_reporting(state);
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->configuration()->diagnostics_codedb_enabled) {
          Diagnostic::stop_reporting(state);
        }
      }
    };
  }
}

#endif
