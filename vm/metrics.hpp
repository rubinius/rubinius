#ifndef RBX_METRICS_HPP
#define RBX_METRICS_HPP

#include "internal_threads.hpp"

#include "gc/root.hpp"

#include "util/timer.hpp"

#include <stdint.h>

#include <string>
#include <vector>

namespace rubinius {
  class VM;
  class State;
  class Tuple;
  class SharedState;

  namespace metrics {
    typedef uint64_t metric;

    struct MetricsData;

    enum MetricsType {
      eNone,
      eRubyMetrics,
      eFinalizerMetrics,
      eJITMetrics,
      eConsoleMetrics,
    };

    struct RubyMetrics {
      // Object memory metrics
      metric memory_young_bytes;
      metric memory_young_objects;
      metric memory_young_occupancy;
      metric memory_immix_bytes;
      metric memory_immix_objects;
      metric memory_immix_chunks;
      metric memory_large_bytes;
      metric memory_large_objects;
      metric memory_symbols;
      metric memory_symbols_bytes;
      metric memory_code_bytes;
      metric memory_promoted_bytes;
      metric memory_promoted_objects;
      metric memory_slab_refills;
      metric memory_slab_refills_fails;
      metric memory_data_objects;
      metric memory_capi_handles;
      metric memory_inflated_headers;

      // Garbage collector metrics
      metric gc_young_count;
      metric gc_young_ms;
      metric gc_immix_count;
      metric gc_immix_stop_ms;
      metric gc_immix_concurrent_ms;
      metric gc_large_count;
      metric gc_large_sweep_ms;

      void init() {
        memory_young_bytes = 0;
        memory_young_objects = 0;
        memory_young_occupancy = 0;
        memory_immix_bytes = 0;
        memory_immix_objects = 0;
        memory_immix_chunks = 0;
        memory_large_bytes = 0;
        memory_large_objects = 0;
        memory_symbols = 0;
        memory_symbols_bytes = 0;
        memory_code_bytes = 0;
        memory_promoted_bytes = 0;
        memory_promoted_objects = 0;
        memory_slab_refills = 0;
        memory_slab_refills_fails = 0;
        memory_data_objects = 0;
        memory_capi_handles = 0;
        memory_inflated_headers = 0;
        gc_young_count = 0;
        gc_young_ms = 0;
        gc_immix_count = 0;
        gc_immix_stop_ms = 0;
        gc_immix_concurrent_ms = 0;
        gc_large_count = 0;
        gc_large_sweep_ms = 0;
      }

      void add(RubyMetrics& data) {
        memory_young_bytes += data.memory_young_bytes;
        memory_young_objects += data.memory_young_objects;
        memory_young_occupancy += data.memory_young_occupancy;
        memory_immix_bytes += data.memory_immix_bytes;
        memory_immix_objects += data.memory_immix_objects;
        memory_immix_chunks += data.memory_immix_chunks;
        memory_large_bytes += data.memory_large_bytes;
        memory_large_objects += data.memory_large_objects;
        memory_symbols += data.memory_symbols;
        memory_symbols_bytes += data.memory_symbols_bytes;
        memory_code_bytes += data.memory_code_bytes;
        memory_promoted_bytes += data.memory_promoted_bytes;
        memory_promoted_objects += data.memory_promoted_objects;
        memory_slab_refills += data.memory_slab_refills;
        memory_slab_refills_fails += data.memory_slab_refills_fails;
        memory_data_objects += data.memory_data_objects;
        memory_capi_handles += data.memory_capi_handles;
        memory_capi_handles += data.memory_capi_handles;
        memory_inflated_headers += data.memory_inflated_headers;
        gc_young_count += data.gc_young_count;
        gc_young_ms += data.gc_young_ms;
        gc_immix_count += data.gc_immix_count;
        gc_immix_stop_ms += data.gc_immix_stop_ms;
        gc_immix_concurrent_ms += data.gc_immix_concurrent_ms;
        gc_large_count += data.gc_large_count;
        gc_large_sweep_ms += data.gc_large_sweep_ms;
      }

      void add(MetricsData& data);
    };

    struct FinalizerMetrics {
      metric objects_queued;
      metric objects_finalized;

      void init() {
        objects_queued = 0;
        objects_finalized = 0;
      }

      void add(FinalizerMetrics& data) {
        objects_queued += data.objects_queued;
        objects_finalized += data.objects_finalized;
      }

      void add(MetricsData& data);
    };

    struct JITMetrics {
      metric methods_queued;
      metric methods_compiled;
      metric methods_failed;
      metric bytes;
      metric time_us;
      metric uncommon_exits;
      metric inlined_accessors;
      metric inlined_methods;
      metric inlined_blocks;

      void init() {
        methods_queued = 0;
        methods_compiled = 0;
        methods_failed = 0;
        bytes = 0;
        time_us = 0;
        uncommon_exits = 0;
        inlined_accessors = 0;
        inlined_methods = 0;
        inlined_blocks = 0;
      }

      void add(JITMetrics& data) {
        methods_queued += data.methods_queued;
        methods_compiled += data.methods_compiled;
        methods_failed += data.methods_failed;
        bytes += data.bytes;
        time_us += data.time_us;
        uncommon_exits += data.uncommon_exits;
        inlined_accessors += data.inlined_accessors;
        inlined_methods += data.inlined_methods;
        inlined_blocks += data.inlined_blocks;
      }

      void add(MetricsData& data);
    };

    struct ConsoleMetrics {
      metric requests_received;
      metric responses_sent;

      void init() {
        requests_received = 0;
        responses_sent = 0;
      }

      void add(ConsoleMetrics& data) {
        requests_received += data.requests_received;
        responses_sent += data.responses_sent;
      }

      void add(MetricsData& data);
    };

    struct SystemMetrics {
      // I/O metrics
      metric io_read_bytes;
      metric io_write_bytes;

      // OS activity metrics
      metric os_signals_received;
      metric os_signals_processed;

      // VM metrics
      metric vm_inline_cache_resets;
      metric vm_threads_created;
      metric vm_threads_destroyed;

      // Lock metrics
      metric locks_stop_the_world_ns;

      void init() {
        io_read_bytes = 0;
        io_write_bytes = 0;
        os_signals_received = 0;
        os_signals_processed = 0;
        vm_inline_cache_resets = 0;
        vm_threads_created = 0;
        vm_threads_destroyed = 0;
        locks_stop_the_world_ns = 0;
      }

      void add(SystemMetrics& data) {
        io_read_bytes += data.io_read_bytes;
        io_write_bytes += data.io_write_bytes;
        os_signals_received += data.os_signals_received;
        os_signals_processed += data.os_signals_processed;
        vm_inline_cache_resets += data.vm_inline_cache_resets;
        vm_threads_created += data.vm_threads_created;
        vm_threads_destroyed += data.vm_threads_destroyed;
        locks_stop_the_world_ns += data.locks_stop_the_world_ns;
      }

      void add(MetricsData& data);
    };

    struct MetricsData {
      MetricsType type;
      union Metrics {
        RubyMetrics ruby_metrics;
        FinalizerMetrics finalizer_metrics;
        JITMetrics jit_metrics;
        ConsoleMetrics console_metrics;
      } m;
      SystemMetrics system_metrics;

      void init(MetricsType mtype) {
        type = mtype;

        switch(type) {
        case eNone:
          break;
        case eRubyMetrics:
          m.ruby_metrics.init();
          break;
        case eFinalizerMetrics:
          m.finalizer_metrics.init();
          break;
        case eJITMetrics:
          m.jit_metrics.init();
          break;
        case eConsoleMetrics:
          m.console_metrics.init();
          break;
        }

        system_metrics.init();
      }
    };

    struct MetricsCollection {
      RubyMetrics ruby_metrics;
      FinalizerMetrics finalizer_metrics;
      JITMetrics jit_metrics;
      ConsoleMetrics console_metrics;
      SystemMetrics system_metrics;

      void init() {
        ruby_metrics.init();
        finalizer_metrics.init();
        jit_metrics.init();
        console_metrics.init();
        system_metrics.init();
      }

      void add(MetricsData& data) {
        switch(data.type) {
        case eNone:
          break;
        case eRubyMetrics:
          ruby_metrics.add(data);
          break;
        case eFinalizerMetrics:
          finalizer_metrics.add(data);
          break;
        case eJITMetrics:
          jit_metrics.add(data);
          break;
        case eConsoleMetrics:
          console_metrics.add(data);
          break;
        }

        system_metrics.add(data);
      }

      void add(MetricsCollection& data) {
        ruby_metrics.add(data.ruby_metrics);
        finalizer_metrics.add(data.finalizer_metrics);
        jit_metrics.add(data.jit_metrics);
        console_metrics.add(data.console_metrics);
        system_metrics.add(data.system_metrics);
      }
    };

    typedef std::pair<std::string, metric&> MetricsItem;
    typedef std::vector<MetricsItem*> MetricsMap;

    class MetricsEmitter {
    public:
      MetricsEmitter() { }
      virtual ~MetricsEmitter() { };

      virtual void send_metrics() = 0;
      virtual void initialize() = 0;
      virtual void cleanup() = 0;
      virtual void reinit() = 0;
    };

    class FileEmitter : public MetricsEmitter {
      MetricsMap& metrics_map_;
      std::string path_;
      int fd_;

    public:
      FileEmitter(MetricsMap& map, std::string path);
      virtual ~FileEmitter();

      void send_metrics();
      void initialize();
      void cleanup();
      void reinit();
    };

    class StatsDEmitter : public MetricsEmitter {
      MetricsMap& metrics_map_;
      std::string host_;
      std::string port_;
      std::string prefix_;
      int socket_fd_;

    public:
      StatsDEmitter(MetricsMap& map, std::string server, std::string prefix);
      virtual ~StatsDEmitter();

      void send_metrics();
      void initialize();
      void cleanup();
      void reinit();
    };

    class Metrics : public InternalThread {
      bool enabled_;

      TypedRoot<Tuple*> values_;

      int interval_;
      utilities::timer::Timer* timer_;
      utilities::thread::Mutex metrics_lock_;

      MetricsCollection metrics_collection_;
      MetricsCollection metrics_history_;

      MetricsMap metrics_map_;

      MetricsEmitter* emitter_;

    public:
      Metrics(STATE);
      virtual ~Metrics();

      void map_metrics();

      void add_historical_metrics(MetricsData& metrics);

      void init_ruby_metrics(STATE);
      void update_ruby_values(STATE);

      void disable(STATE) {
        enabled_ = false;
      }

      void initialize(STATE);
      void run(STATE);
      void wakeup(STATE);
      void after_fork_child(STATE);
    };
  }
}

#endif
