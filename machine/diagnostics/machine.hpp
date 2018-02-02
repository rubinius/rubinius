#ifndef RBX_DIAGNOSTICS_MACHINE_HPP
#define RBX_DIAGNOSTICS_MACHINE_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class BootMetrics {
    public:
      metric fields_us;
      metric main_thread_us;
      metric memory_us;
      metric ontology_us;
      metric platform_us;

      BootMetrics() {
        fields_us = 0;
        main_thread_us = 0;
        memory_us = 0;
        ontology_us = 0;
        platform_us = 0;
      }
    };

    class MachineMetrics {
    public:
      metric checkpoints;
      metric stops;
      metric call_site_count;
      metric inline_cache_count;
      metric inline_cache_disabled;
      metric cache_resets;
      metric methods_invoked;
      metric blocks_invoked;
      metric backtraces;
      metric backtrace_us;
      metric bytecode_load_us;
      metric bytecode_verifier_us;
      metric bytecode_internalizer_us;
      metric samples;
      metric sample_ns;
      metric profiles;
      metric profile_ns;
      metric unwind_site_count;
      metric unwind_site_full;
      metric unwind_site_polymorphic;
      metric read_bytes;
      metric write_bytes;
      metric signals_received;
      metric signals_processed;
      metric threads_created;
      metric threads_destroyed;
      metric fibers_created;
      metric fibers_destroyed;

      MachineMetrics() {
        checkpoints = 0;
        stops = 0;
        call_site_count = 0;
        inline_cache_count = 0;
        inline_cache_disabled = 0;
        cache_resets = 0;
        methods_invoked = 0;
        blocks_invoked = 0;
        backtraces = 0;
        backtrace_us = 0;
        bytecode_load_us = 0;
        bytecode_verifier_us = 0;
        bytecode_internalizer_us = 0;
        samples = 0;
        sample_ns = 0;
        profiles = 0;
        profile_ns = 0;
        unwind_site_count = 0;
        unwind_site_full = 0;
        unwind_site_polymorphic = 0;
        read_bytes = 0;
        write_bytes = 0;
        signals_received = 0;
        signals_processed = 0;
        threads_created = 0;
        threads_destroyed = 0;
        fibers_created = 0;
        fibers_destroyed = 0;
      }
    };
  }
}

#endif
