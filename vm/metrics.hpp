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
      metric memory_data_objects_total;
      metric memory_capi_handles;
      metric memory_capi_handles_total;
      metric memory_inflated_headers;

      // Garbage collector metrics
      metric gc_young_count;
      metric gc_young_last_ms;
      metric gc_young_total_ms;
      metric gc_young_lifetime;
      metric gc_immix_count;
      metric gc_immix_stop_last_ms;
      metric gc_immix_stop_total_ms;
      metric gc_immix_conc_last_ms;
      metric gc_immix_conc_total_ms;
      metric gc_large_count;
      metric gc_large_sweep_last_ms;
      metric gc_large_sweep_total_ms;

      void init() {
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
        memory_data_objects_total = 0;
        memory_capi_handles = 0;
        memory_capi_handles_total = 0;
        memory_inflated_headers = 0;
        gc_young_count = 0;
        gc_young_last_ms = 0;
        gc_young_total_ms = 0;
        gc_young_lifetime = 0;
        gc_immix_count = 0;
        gc_immix_stop_last_ms = 0;
        gc_immix_stop_total_ms = 0;
        gc_immix_conc_last_ms = 0;
        gc_immix_conc_total_ms = 0;
        gc_large_count = 0;
        gc_large_sweep_last_ms = 0;
        gc_large_sweep_total_ms = 0;
      }

      void add(RubyMetrics* data) {
        memory_young_bytes += data->memory_young_bytes;
        memory_young_bytes_total += data->memory_young_bytes_total;
        memory_young_objects += data->memory_young_objects;
        memory_young_objects_total += data->memory_young_objects_total;
        memory_young_percent_used += data->memory_young_percent_used;
        memory_immix_bytes += data->memory_immix_bytes;
        memory_immix_bytes_total += data->memory_immix_bytes_total;
        memory_immix_objects += data->memory_immix_objects;
        memory_immix_objects_total += data->memory_immix_objects_total;
        memory_immix_chunks += data->memory_immix_chunks;
        memory_immix_chunks_total += data->memory_immix_chunks_total;
        memory_large_bytes += data->memory_large_bytes;
        memory_large_bytes_total += data->memory_large_bytes_total;
        memory_large_objects += data->memory_large_objects;
        memory_large_objects_total += data->memory_large_objects_total;
        memory_symbols_bytes += data->memory_symbols_bytes;
        memory_code_bytes += data->memory_code_bytes;
        memory_jit_bytes += data->memory_jit_bytes;
        memory_promoted_bytes_total += data->memory_promoted_bytes_total;
        memory_promoted_objects_total += data->memory_promoted_objects_total;
        memory_slab_refills_total += data->memory_slab_refills_total;
        memory_slab_refills_fails += data->memory_slab_refills_fails;
        memory_data_objects_total += data->memory_data_objects_total;
        memory_capi_handles += data->memory_capi_handles;
        memory_capi_handles_total += data->memory_capi_handles_total;
        memory_inflated_headers += data->memory_inflated_headers;
        gc_young_count += data->gc_young_count;
        gc_young_last_ms += data->gc_young_last_ms;
        gc_young_total_ms += data->gc_young_total_ms;
        gc_young_lifetime += data->gc_young_lifetime;
        gc_immix_count += data->gc_immix_count;
        gc_immix_stop_last_ms += data->gc_immix_stop_last_ms;
        gc_immix_stop_total_ms += data->gc_immix_stop_total_ms;
        gc_immix_conc_last_ms += data->gc_immix_conc_last_ms;
        gc_immix_conc_total_ms += data->gc_immix_conc_total_ms;
        gc_large_count += data->gc_large_count;
        gc_large_sweep_last_ms += data->gc_large_sweep_last_ms;
        gc_large_sweep_total_ms += data->gc_large_sweep_total_ms;
      }

      void add(MetricsData* data);
    };

    struct FinalizerMetrics {
      metric objects_queued;
      metric objects_finalized;

      void init() {
        objects_queued = 0;
        objects_finalized = 0;
      }

      void add(FinalizerMetrics* data) {
        objects_queued += data->objects_queued;
        objects_finalized += data->objects_finalized;
      }

      void add(MetricsData* data);
    };

    struct JITMetrics {
      metric methods_queued;
      metric methods_compiled;
      metric methods_failed;
      metric time_last_us;
      metric time_total_us;

      void init() {
        methods_queued = 0;
        methods_compiled = 0;
        methods_failed = 0;
        time_last_us = 0;
        time_total_us = 0;
      }

      void add(JITMetrics* data) {
        methods_queued += data->methods_queued;
        methods_compiled += data->methods_compiled;
        methods_failed += data->methods_failed;
        time_last_us += data->time_last_us;
        time_total_us += data->time_total_us;
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

      void add(ConsoleMetrics* data) {
        requests_received += data->requests_received;
        responses_sent += data->responses_sent;
      }

      void add(MetricsData* data);
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
      metric vm_threads;
      metric vm_threads_total;

      // Lock metrics
      metric locks_stop_the_world_last_ns;
      metric locks_stop_the_world_total_ns;

      void init() {
        io_read_bytes = 0;
        io_write_bytes = 0;
        os_signals_received = 0;
        os_signals_processed = 0;
        vm_inline_cache_resets = 0;
        vm_threads = 0;
        vm_threads_total = 0;
        locks_stop_the_world_last_ns = 0;
        locks_stop_the_world_total_ns = 0;
      }

      void add(SystemMetrics* data) {
        io_read_bytes += data->io_read_bytes;
        io_write_bytes += data->io_write_bytes;
        os_signals_received += data->os_signals_received;
        os_signals_processed += data->os_signals_processed;
        vm_inline_cache_resets += data->vm_inline_cache_resets;
        vm_threads += data->vm_threads;
        vm_threads_total += data->vm_threads_total;
        locks_stop_the_world_last_ns += data->locks_stop_the_world_last_ns;
        locks_stop_the_world_total_ns += data->locks_stop_the_world_total_ns;
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

      void add(MetricsData* data) {
        switch(data->type) {
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

      void add(MetricsCollection* data) {
        ruby_metrics.add(&data->ruby_metrics);
        finalizer_metrics.add(&data->finalizer_metrics);
        jit_metrics.add(&data->jit_metrics);
        console_metrics.add(&data->console_metrics);
        system_metrics.add(&data->system_metrics);
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

      void wakeup();

      void add_historical_metrics(MetricsData* metrics);

      void init_ruby_metrics(STATE);
      void update_ruby_values(STATE);

      void start(STATE);

      void start_thread(STATE);
      void stop_thread(STATE);

      void shutdown(STATE);
      void after_fork_child(STATE);

      void process_metrics(STATE);
    };
  }
}

#endif
