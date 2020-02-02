#ifndef RBX_DIAGNOSTICS_HPP
#define RBX_DIAGNOSTICS_HPP

#include "machine_thread.hpp"

#include "util/timer.hpp"

#include <list>
#include <mutex>
#include <unordered_set>

namespace rubinius {
  class Configuration;
  class Diagnostics;

  namespace diagnostics {
    class Diagnostic;
    class Emitter;
    class BootMetrics;
    class CodeDBMetrics;
    class CodeManager;
    class CollectorMetrics;
    class Handles;
    class Immix;
    class ExtendedHeader;
    class LargeRegion;
    class MachineMetrics;
    class Memory;
    class MemoryMetrics;
    class Profiler;
    class SymbolTable;

    typedef uint64_t metric;

    typedef std::unordered_set<Diagnostic*> RecurringReports;
    typedef std::list<Diagnostic*> IntermittentReports;

    class Reporter : public MachineThread {
      Diagnostics* diagnostics_;

      utilities::timer::Timer* timer_;

      Emitter* emitter_;

    public:
      Reporter(STATE, Diagnostics* d);

      ~Reporter() {
        if(timer_) {
          delete timer_;
          timer_ = nullptr;
        }
      }

      void initialize(STATE);
      void run(STATE);
      void wakeup(STATE);
      void after_fork_child(STATE);

      void report();
    };
  }

  class Diagnostics {
    diagnostics::BootMetrics* boot_metrics_;
    diagnostics::CodeDBMetrics* codedb_metrics_;
    diagnostics::CollectorMetrics* collector_metrics_;
    diagnostics::MemoryMetrics* memory_metrics_;

    diagnostics::RecurringReports recurring_reports_;
    diagnostics::IntermittentReports intermittent_reports_;

    diagnostics::Reporter* reporter_;

    std::mutex lock_;

    int interval_;

  public:
    Diagnostics(Configuration* configuration);

    ~Diagnostics() {
      if(reporter_) {
        delete reporter_;
        reporter_ = nullptr;
      }
    }

    std::mutex& lock() {
      return lock_;
    }

    int interval() const {
      return interval_;
    }

    diagnostics::BootMetrics* boot_metrics() {
      return boot_metrics_;
    }

    diagnostics::CodeDBMetrics* codedb_metrics() {
      return codedb_metrics_;
    }

    diagnostics::CollectorMetrics* collector_metrics() {
      return collector_metrics_;
    }

    diagnostics::MemoryMetrics* memory_metrics() {
      return memory_metrics_;
    }

    diagnostics::RecurringReports& recurring_reports() {
      return recurring_reports_;
    }

    diagnostics::IntermittentReports& intermittent_reports() {
      return intermittent_reports_;
    }

    void start_reporter(STATE) {
      if(!reporter_) {
        reporter_ = new diagnostics::Reporter(state, this);
        reporter_->start(state);
      }
    }

    void report(diagnostics::Diagnostic* diagnostic) {
      std::lock_guard<std::mutex> guard(lock_);

      intermittent_reports_.push_back(diagnostic);

      if(reporter_) {
        reporter_->report();
      }
    }

    void add_report(STATE, diagnostics::Diagnostic* diagnostic) {
      std::lock_guard<std::mutex> guard(lock_);

      recurring_reports_.insert(diagnostic);

      start_reporter(state);
    }

    void remove_report(diagnostics::Diagnostic* diagnostic) {
      std::lock_guard<std::mutex> guard(lock_);

      recurring_reports_.erase(diagnostic);
    }
  };
}

#endif
