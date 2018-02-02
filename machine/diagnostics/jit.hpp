#ifndef RBX_DIAGNOSTICS_JIT_HPP
#define RBX_DIAGNOSTICS_JIT_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class JITMetrics {
    public:
      metric methods_queued;
      metric methods_compiled;
      metric methods_failed;
      metric compile_time_us;
      metric uncommon_exits;
      metric inlined_accessors;
      metric inlined_methods;
      metric inlined_blocks;
      metric inlined_primitives;
      metric inlined_ffi;

      JITMetrics() {
        methods_queued = 0;
        methods_compiled = 0;
        methods_failed = 0;
        compile_time_us = 0;
        uncommon_exits = 0;
        inlined_accessors = 0;
        inlined_methods = 0;
        inlined_blocks = 0;
        inlined_primitives = 0;
        inlined_ffi = 0;
      }
    };
  }
}

#endif
