#ifndef RBX_DIAGNOSTICS_GC_HPP
#define RBX_DIAGNOSTICS_GC_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class CollectorMetrics : public Diagnostic {
    public:
      metric first_region_set;
      metric first_region_count;
      metric first_region_stop_ms;
      metric first_region_suspend_ms;
      metric first_region_concurrent_ms;
      metric first_region_diagnostics_us;
      metric large_set;
      metric large_count;
      metric large_sweep_us;
      metric objects_queued;
      metric objects_finalized;
      metric headers_set;
      metric handles_set;
      metric resource_set;

      CollectorMetrics()
        : Diagnostic()
        , first_region_set(0)
        , first_region_count(0)
        , first_region_stop_ms(0)
        , first_region_suspend_ms(0)
        , first_region_concurrent_ms(0)
        , first_region_diagnostics_us(0)
        , large_set(0)
        , large_count(0)
        , large_sweep_us(0)
        , objects_queued(0)
        , objects_finalized(0)
        , headers_set(0)
        , handles_set(0)
        , resource_set(0)
      {
        set_type("CollectorMetrics");

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_.AddMember("first_region_set", first_region_set, alloc);
        document_.AddMember("first_region_count", first_region_count, alloc);
        document_.AddMember("first_region_stop_ms", first_region_stop_ms, alloc);
        document_.AddMember("first_region_suspend_ms", first_region_suspend_ms, alloc);
        document_.AddMember("first_region_concurrent_ms", first_region_concurrent_ms, alloc);
        document_.AddMember("first_region_diagnostics_us", first_region_diagnostics_us, alloc);
        document_.AddMember("large_set", large_set, alloc);
        document_.AddMember("large_count", large_count, alloc);
        document_.AddMember("large_sweep_us", large_sweep_us, alloc);
        document_.AddMember("objects_queued", objects_queued, alloc);
        document_.AddMember("objects_finalized", objects_finalized, alloc);
        document_.AddMember("headers_set", headers_set, alloc);
        document_.AddMember("handles_set", handles_set, alloc);
        document_.AddMember("resource_set", resource_set, alloc);
      }

      virtual ~CollectorMetrics() { }

      virtual void update() {
        document_["first_region_set"] = first_region_set;
        document_["first_region_count"] = first_region_count;
        document_["first_region_stop_ms"] = first_region_stop_ms;
        document_["first_region_suspend_ms"] = first_region_suspend_ms;
        document_["first_region_concurrent_ms"] = first_region_concurrent_ms;
        document_["first_region_diagnostics_us"] = first_region_diagnostics_us;
        document_["large_set"] = large_set;
        document_["large_count"] = large_count;
        document_["large_sweep_us"] = large_sweep_us;
        document_["objects_queued"] = objects_queued;
        document_["objects_finalized"] = objects_finalized;
        document_["headers_set"] = headers_set;
        document_["handles_set"] = handles_set;
        document_["resource_set"] = resource_set;
      }

      virtual void start_reporting(STATE) {
        if(state->configuration()->diagnostics_collector_enabled) {
          Diagnostic::start_reporting(state);
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->configuration()->diagnostics_collector_enabled) {
          Diagnostic::stop_reporting(state);
        }
      }
    };
  }
}

#endif
