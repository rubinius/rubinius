#ifndef RBX_DIAGNOSTICS_HPP
#define RBX_DIAGNOSTICS_HPP

#include "machine_threads.hpp"

#include "util/timer.hpp"

#include <list>
#include <mutex>
#include <unordered_set>

namespace rubinius {
  namespace diagnostics {
    class Diagnostics;
    class Diagnostic;
    class Emitter;
    class BootMetrics;
    class CodeDBMetrics;
    class CodeManager;
    class GCMetrics;
    class Handles;
    class Immix;
    class ExtendedHeader;
    class MachineMetrics;
    class MarkSweep;
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

    class Diagnostics {
      RecurringReports recurring_reports_;
      IntermittentReports intermittent_reports_;

      Reporter* reporter_;

      std::mutex lock_;

      int interval_;

    public:
      Diagnostics(STATE);

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

      RecurringReports& recurring_reports() {
        return recurring_reports_;
      }

      IntermittentReports& intermittent_reports() {
        return intermittent_reports_;
      }

      void start_reporter(STATE) {
        if(!reporter_) {
          reporter_ = new Reporter(state, this);
          reporter_->start(state);
        }
      }

      void report(Diagnostic* diagnostic) {
        std::lock_guard<std::mutex> guard(lock_);

        intermittent_reports_.push_back(diagnostic);

        if(reporter_) {
          reporter_->report();
        }
      }

      void add_report(STATE, Diagnostic* diagnostic) {
        std::lock_guard<std::mutex> guard(lock_);

        recurring_reports_.insert(diagnostic);

        start_reporter(state);
      }

      void remove_report(Diagnostic* diagnostic) {
        std::lock_guard<std::mutex> guard(lock_);

        recurring_reports_.erase(diagnostic);
      }
    };
  }
}

#endif
