#ifndef RBX_DIAGNOSTICS_HPP
#define RBX_DIAGNOSTICS_HPP

#include "machine_threads.hpp"

#include "util/timer.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#include <stdint.h>
#include <mutex>
#include <unordered_set>

namespace rubinius {
  namespace diagnostics {
    class Measurement;

    class DiagnosticsData {
    public:
      rapidjson::Document document;

      DiagnosticsData();
      virtual ~DiagnosticsData() { }

      virtual void update() { }

      void set_type(const char* type);
      void to_string(rapidjson::StringBuffer& buffer);
    };

    class Formatter {

    public:
      Formatter() { }
      virtual ~Formatter();


    };

    class Emitter {
    public:
      virtual void send_diagnostics(DiagnosticsData*) = 0;
    };

    class FileEmitter : public Emitter {
      std::string path_;
      int fd_;

    public:
      FileEmitter(STATE, std::string path);

      void send_diagnostics(DiagnosticsData* data);
    };

    typedef std::unordered_set<Measurement*> Measurements;

    class Diagnostics;

    class Reporter : public MachineThread {
      utilities::timer::Timer* timer_;
      int interval_;

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
