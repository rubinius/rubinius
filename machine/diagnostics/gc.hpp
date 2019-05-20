#ifndef RBX_DIAGNOSTICS_GC_HPP
#define RBX_DIAGNOSTICS_GC_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class GCMetrics : public Diagnostic {
    public:
      metric young_set;
      metric young_count;
      metric young_ms;
      metric immix_set;
      metric immix_count;
      metric immix_stop_ms;
      metric immix_suspend_ms;
      metric immix_concurrent_ms;
      metric immix_diagnostics_us;
      metric large_set;
      metric large_count;
      metric large_sweep_us;
      metric objects_queued;
      metric objects_finalized;
      metric headers_set;
      metric handles_set;
      metric resource_set;

      GCMetrics()
        : Diagnostic()
        , young_set(0)
        , young_count(0)
        , young_ms(0)
        , immix_set(0)
        , immix_count(0)
        , immix_stop_ms(0)
        , immix_suspend_ms(0)
        , immix_concurrent_ms(0)
        , immix_diagnostics_us(0)
        , large_set(0)
        , large_count(0)
        , large_sweep_us(0)
        , objects_queued(0)
        , objects_finalized(0)
        , headers_set(0)
        , handles_set(0)
        , resource_set(0)
      {
        set_type("GCMetrics");

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_.AddMember("young_set", young_set, alloc);
        document_.AddMember("young_count", young_count, alloc);
        document_.AddMember("young_ms", young_ms, alloc);
        document_.AddMember("immix_set", immix_set, alloc);
        document_.AddMember("immix_count", immix_count, alloc);
        document_.AddMember("immix_stop_ms", immix_stop_ms, alloc);
        document_.AddMember("immix_suspend_ms", immix_suspend_ms, alloc);
        document_.AddMember("immix_concurrent_ms", immix_concurrent_ms, alloc);
        document_.AddMember("immix_diagnostics_us", immix_diagnostics_us, alloc);
        document_.AddMember("large_set", large_set, alloc);
        document_.AddMember("large_count", large_count, alloc);
        document_.AddMember("large_sweep_us", large_sweep_us, alloc);
        document_.AddMember("objects_queued", objects_queued, alloc);
        document_.AddMember("objects_finalized", objects_finalized, alloc);
        document_.AddMember("headers_set", headers_set, alloc);
        document_.AddMember("handles_set", handles_set, alloc);
        document_.AddMember("resource_set", resource_set, alloc);
      }

      virtual ~GCMetrics() { }

      virtual void update() {
        document_["young_set"] = young_set;
        document_["young_count"] = young_count;
        document_["young_ms"] = young_ms;
        document_["immix_set"] = immix_set;
        document_["immix_count"] = immix_count;
        document_["immix_stop_ms"] = immix_stop_ms;
        document_["immix_suspend_ms"] = immix_suspend_ms;
        document_["immix_concurrent_ms"] = immix_concurrent_ms;
        document_["immix_diagnostics_us"] = immix_diagnostics_us;
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
        if(state->shared().config.diagnostics_gc_enabled) {
          Diagnostic::start_reporting(state);
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->shared().config.diagnostics_gc_enabled) {
          Diagnostic::stop_reporting(state);
        }
      }
    };
  }
}

#endif
