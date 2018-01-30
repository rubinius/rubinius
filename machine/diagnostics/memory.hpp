#ifndef RBX_DIAGNOSTICS_MEMORY_HPP
#define RBX_DIAGNOSTICS_MEMORY_HPP

#include "diagnostics/metrics.hpp"

namespace rubinius {
  namespace diagnostics {
    class MemoryDiagnostics {
    public:
      uint64_t objects_;
      uint64_t bytes_;

      MemoryDiagnostics();
      virtual ~MemoryDiagnostics() { }
    };

    struct MemoryMetrics {
      metric young_bytes;
      metric young_objects;
      metric immix_bytes;
      metric immix_objects;
      metric immix_chunks;
      metric large_bytes;
      metric large_objects;
      metric symbols;
      metric symbols_bytes;
      metric code_bytes;
      metric jit_bytes;
      metric promoted_bytes;
      metric promoted_objects;
      metric slab_refills;
      metric slab_refills_fails;
      metric data_objects;
      metric capi_handles;
      metric inflated_headers;
      metric suspend_ms;
      metric allocated_bytes;
      metric malloc;
      metric calloc;
      metric realloc;
      metric freed;

      MemoryMetrics() {
        young_bytes = 0;
        young_objects = 0;
        immix_bytes = 0;
        immix_objects = 0;
        immix_chunks = 0;
        large_bytes = 0;
        large_objects = 0;
        symbols = 0;
        symbols_bytes = 0;
        code_bytes = 0;
        jit_bytes = 0;
        promoted_bytes = 0;
        promoted_objects = 0;
        slab_refills = 0;
        slab_refills_fails = 0;
        data_objects = 0;
        capi_handles = 0;
        inflated_headers = 0;
        suspend_ms = 0;
        allocated_bytes = 0;
        malloc = 0;
        calloc = 0;
        realloc = 0;
        freed = 0;
      }
    };
  }
}

#endif
