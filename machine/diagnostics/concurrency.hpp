#ifndef RBX_DIAGNOSTICS_CONCURRENCY_HPP
#define RBX_DIAGNOSTICS_CONCURRENCY_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    struct ConcurrencyMetrics : public Diagnostic {
      metric stop_the_world_ns;

      ConcurrencyMetrics()
        : Diagnostic()
        , stop_the_world_ns(0)
      {
        set_type("ConcurrencyMetrics");

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_.AddMember("stop_the_world.ns", stop_the_world_ns, alloc);
      }

      virtual ~ConcurrencyMetrics() { }

      virtual void update() {
        document_["stop_the_world.ns"] = stop_the_world_ns;
      }

      virtual void start_reporting(STATE) {
        if(state->configuration()->diagnostics_machine_enabled) {
          Diagnostic::start_reporting(state);
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->configuration()->diagnostics_machine_enabled) {
          Diagnostic::stop_reporting(state);
        }
      }
    };
  }
}

#endif
