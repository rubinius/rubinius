#ifndef RBX_DIAGNOSTICS_GC_HPP
#define RBX_DIAGNOSTICS_GC_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class GCMetrics {
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

      GCMetrics() {
        young_set = 0;
        young_count = 0;
        young_ms = 0;
        immix_set = 0;
        immix_count = 0;
        immix_stop_ms = 0;
        immix_suspend_ms = 0;
        immix_concurrent_ms = 0;
        immix_diagnostics_us = 0;
        large_set = 0;
        large_count = 0;
        large_sweep_us = 0;
        objects_queued = 0;
        objects_finalized = 0;
        headers_set = 0;
        handles_set = 0;
        resource_set = 0;
      }
    };
  }
}

#endif
