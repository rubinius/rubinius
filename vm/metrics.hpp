#ifndef RBX_METRICS_HPP
#define RBX_METRICS_HPP

#include "lock.hpp"
#include "auxiliary_threads.hpp"

#include "gc/root.hpp"

#include "util/timer.hpp"

#include <stdint.h>

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
      eImmixMetrics,
      eJITMetrics,
      eConsoleMetrics,
    };

    struct RubyMetrics {
      metric bytes_read;
      metric bytes_written;
      metric signals_received;
      metric signals_processed;

      void init() {
        bytes_read = 0;
        bytes_written = 0;
        signals_received = 0;
        signals_processed = 0;
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

      void add(MetricsData* data);
    };

    struct ImmixMetrics {
      void init() {
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
      void init() {
      }

      void add(MetricsData* data);
    };

    struct MetricsData {
      MetricsType type;
      union Metrics {
        RubyMetrics ruby_metrics;
        FinalizerMetrics finalizer_metrics;
        ImmixMetrics immix_metrics;
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
        case eImmixMetrics:
          m.immix_metrics.init();
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
      ImmixMetrics immix_metrics;
      JITMetrics jit_metrics;
      ConsoleMetrics console_metrics;

      void init() {
        ruby_metrics.init();
        finalizer_metrics.init();
        immix_metrics.init();
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
        case eImmixMetrics:
          immix_metrics.add(data);
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

    class Metrics : public AuxiliaryThread, public Lockable {
      SharedState& shared_;
      VM* vm_;
      bool thread_exit_;

      TypedRoot<Thread*> thread_;

      int interval_;
      utilities::timer::Timer* timer_;

      MetricsCollection metrics_collection_;
      MetricsCollection metrics_history_;

    public:
      Metrics(STATE);
      virtual ~Metrics();

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
