#ifndef RBX_DIAGNOSTICS_HPP
#define RBX_DIAGNOSTICS_HPP

#include "machine_threads.hpp"

#include "util/timer.hpp"

#include <list>
#include <mutex>
#include <unordered_set>

namespace rubinius {
  namespace diagnostics {
    class Emitter;
    class Formatter;
    class ImmixFormatter;
    class Measurement;

    typedef std::unordered_set<Measurement*> Measurements;
    typedef std::list<Formatter*> ReportRequests;

    class Diagnostics;

    class Reporter : public MachineThread {
      utilities::timer::Timer* timer_;
      int interval_;

      ReportRequests list_;
      Emitter* emitter_;

      std::mutex diagnostics_lock_;

      Diagnostics* diagnostics_;

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

      void report(Formatter* formatter);
    };

    class Diagnostics {
      Measurements measurements_;
      Reporter* reporter_;

    public:
      Diagnostics()
        : measurements_()
        , reporter_(nullptr)
      {
      }

      ~Diagnostics() {
        if(reporter_) {
          delete reporter_;
          reporter_ = nullptr;
        }
      }

      Measurements& measurements() {
        return measurements_;
      }

      void start_reporter(STATE) {
        if(!reporter_) {
          reporter_ = new Reporter(state, this);
          reporter_->start(state);
        }
      }

      void report(Formatter* formatter) {
        if(reporter_) {
          reporter_->report(formatter);
        }
      }

      void add_measurement(Measurement* m) {
        measurements_.insert(m);
      }
    };
  }
}

#endif
