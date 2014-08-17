#ifndef RBX_METRICS_HPP
#define RBX_METRICS_HPP

#include "lock.hpp"
#include "auxiliary_threads.hpp"

#include "gc/root.hpp"

#include "util/timer.hpp"

#include <stdint.h>

#include <string>
#include <vector>

namespace rubinius {
  class VM;
  class State;
  class Thread;

  namespace metrics {
    typedef uint64_t metric;

    struct MetricsData;

    enum MetricsType {
      eRubyMetrics,
      eFinalizerMetrics,
      eJITMetrics,
      eConsoleMetrics,
    };

    struct RubyMetrics {
      // IO metrics
      metric io_read_bytes;
      metric io_write_bytes;

      // OS activity metrics
      metric os_signals_received;
      metric os_signals_processed;

      // VM metrics
      metric inline_cache_resets;

      // Object memory metrics
      metric memory_young_bytes;
      metric memory_young_bytes_total;
      metric memory_young_objects;
      metric memory_young_objects_total;
      metric memory_young_percent_used;
      metric memory_immix_bytes;
      metric memory_immix_bytes_total;
      metric memory_immix_objects;
      metric memory_immix_objects_total;
      metric memory_immix_chunks;
      metric memory_immix_chunks_total;
      metric memory_large_bytes;
      metric memory_large_bytes_total;
      metric memory_large_objects;
      metric memory_large_objects_total;
      metric memory_symbols_bytes;
      metric memory_code_bytes;
      metric memory_jit_bytes;
      metric memory_promoted_bytes_total;
      metric memory_promoted_objects_total;
      metric memory_slab_refills_total;
      metric memory_slab_refills_fails;
      metric data_objects_total;
      metric capi_handles;
      metric capi_handles_total;
      metric inflated_headers;


      // Garbage collector metrics
      metric gc_young_count;
      metric gc_young_last_ms;
      metric gc_young_total_ms;
      metric gc_immix_count;
      metric gc_immix_stop_last_ms;
      metric gc_immix_stop_total_ms;
      metric gc_immix_conc_last_ms;
      metric gc_immix_conc_total_ms;
      metric gc_large_count;
      metric gc_large_sweep_last_ms;
      metric gc_large_sweep_total_ms;

      void init() {
        io_read_bytes = 0;
        io_write_bytes = 0;
        os_signals_received = 0;
        os_signals_processed = 0;
        inline_cache_resets = 0;
        memory_young_bytes = 0;
        memory_young_bytes_total = 0;
        memory_young_objects = 0;
        memory_young_objects_total = 0;
        memory_young_percent_used = 0;
        memory_immix_bytes = 0;
        memory_immix_bytes_total = 0;
        memory_immix_objects = 0;
        memory_immix_objects_total = 0;
        memory_immix_chunks = 0;
        memory_immix_chunks_total = 0;
        memory_large_bytes = 0;
        memory_large_bytes_total = 0;
        memory_large_objects = 0;
        memory_large_objects_total = 0;
        memory_symbols_bytes = 0;
        memory_code_bytes = 0;
        memory_jit_bytes = 0;
        memory_promoted_bytes_total = 0;
        memory_promoted_objects_total = 0;
        memory_slab_refills_total = 0;
        memory_slab_refills_fails = 0;
        data_objects_total = 0;
        capi_handles = 0;
        capi_handles_total = 0;
        inflated_headers = 0;
        gc_young_count = 0;
        gc_young_last_ms = 0;
        gc_young_total_ms = 0;
        gc_immix_count = 0;
        gc_immix_stop_last_ms = 0;
        gc_immix_stop_total_ms = 0;
        gc_immix_conc_last_ms = 0;
        gc_immix_conc_total_ms = 0;
        gc_large_count = 0;
        gc_large_sweep_last_ms = 0;
        gc_large_sweep_total_ms = 0;
      }

      void add(MetricsData* data);
      void reset();
    };

    struct FinalizerMetrics {
      metric objects_queued;
      metric objects_finalized;

      void init() {
        objects_queued = 0;
        objects_finalized = 0;
      }

      void add(MetricsData* data);
    };

    struct JITMetrics {
      metric methods_queued;
      metric methods_compiled;

      void init() {
        methods_queued = 0;
        methods_compiled = 0;
      }

      void add(MetricsData* data);
    };

    struct ConsoleMetrics {
      metric requests_received;
      metric responses_sent;

      void init() {
        requests_received = 0;
        responses_sent = 0;
      }

      void add(MetricsData* data);
    };

    struct MetricsData {
      MetricsType type;
      union Metrics {
        RubyMetrics ruby_metrics;
        FinalizerMetrics finalizer_metrics;
        JITMetrics jit_metrics;
        ConsoleMetrics console_metrics;
      } m;

      void init(MetricsType mtype) {
        type = mtype;

        switch(type) {
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
      }
    };

    struct MetricsCollection {
      RubyMetrics ruby_metrics;
      FinalizerMetrics finalizer_metrics;
      JITMetrics jit_metrics;
      ConsoleMetrics console_metrics;

      void init() {
        ruby_metrics.init();
        finalizer_metrics.init();
        jit_metrics.init();
        console_metrics.init();
      }

      void add(MetricsData* data) {
        switch(data->type) {
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

    class Metrics : public AuxiliaryThread, public Lockable {
      SharedState& shared_;
      VM* vm_;
      bool thread_exit_;

      TypedRoot<Thread*> thread_;

      int interval_;
      utilities::timer::Timer* timer_;

      MetricsCollection metrics_collection_;
      MetricsCollection metrics_history_;

      MetricsMap metrics_map_;

      MetricsEmitter* emitter_;

    public:
      Metrics(STATE);
      virtual ~Metrics();

      void map_metrics();

      void wakeup();

      void start(STATE);

      void start_thread(STATE);
      void stop_thread(STATE);

      void shutdown(STATE);
      void before_exec(STATE);
      void after_exec(STATE);
      void before_fork(STATE);
      void after_fork_parent(STATE);
      void after_fork_child(STATE);

      void process_metrics(STATE);
    };
  }
}

#endif
