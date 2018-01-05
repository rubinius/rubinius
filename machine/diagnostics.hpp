#ifndef RBX_DIAGNOSTICS_HPP
#define RBX_DIAGNOSTICS_HPP

#include "machine_threads.hpp"

#include "util/thread.hpp"
#include "util/timer.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#include <atomic>
#include <stdint.h>
#include <list>
#include <unordered_set>

namespace rubinius {
  namespace diagnostics {
    class Measurement {
    public:
      typedef void (Update)(STATE, Measurement*, intptr_t value);
      typedef void (Scan)(STATE, Measurement*);
      typedef std::string (Report)(STATE, Measurement*);

      std::atomic<intptr_t> _value_;

      attr_field(update, Update*);
      attr_field(scan, Scan*);
      attr_field(report, Report*);

      Measurement()
        : _value_(0)
        , _update_(nullptr)
        , _scan_(nullptr)
        , _report_(nullptr)
      {
      }

      static Measurement* create_counter(STATE);

      static void update_counter(STATE, Measurement* m, intptr_t value) {
        m->_value_.fetch_add(value);
      }

      intptr_t value() {
        return _value_;
      }

      void update(STATE, intptr_t value) {
        if(_update_) _update_(state, this, value);
      }

      void scan(STATE) {
        if(_scan_) _scan_(state, this);
      }

      std::string report(STATE) {
        if(_report_) {
          return _report_(state, this);
        } else {
          return std::string("");
        }
      }
    };

    class DiagnosticsData {
    public:
      rapidjson::Document document;

      DiagnosticsData();
      virtual ~DiagnosticsData() { }

      virtual void update() { }

      void set_type(const char* type);
      void to_string(rapidjson::StringBuffer& buffer);
    };

    class MemoryDiagnostics : public DiagnosticsData {
    public:
      uint64_t objects_;
      uint64_t bytes_;

      MemoryDiagnostics();
      virtual ~MemoryDiagnostics() { }
    };

    class DiagnosticsEmitter {
    public:
      virtual void send_diagnostics(DiagnosticsData*) = 0;
    };

    class FileEmitter : public DiagnosticsEmitter {
      std::string path_;
      int fd_;

    public:
      FileEmitter(STATE, std::string path);

      void send_diagnostics(DiagnosticsData* data);
    };

    typedef std::list<DiagnosticsData*> DiagnosticsList;
    typedef std::unordered_set<Measurement*> Measurements;

    class DiagnosticsReporter : public MachineThread {
      utilities::timer::Timer* timer_;
      int interval_;

      DiagnosticsList list_;

      DiagnosticsEmitter* emitter_;

      utilities::thread::Mutex diagnostics_lock_;

    public:
      DiagnosticsReporter(STATE);
      ~DiagnosticsReporter() {
        if(timer_) {
          delete timer_;
          timer_ = nullptr;
        }
      }

      void initialize(STATE);
      void run(STATE);
      void wakeup(STATE);
      void after_fork_child(STATE);

      void report(DiagnosticsData* data);
    };

    class Diagnostics {
      Measurements measurements_;
      DiagnosticsReporter* reporter_;

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

      void start_reporter(STATE) {
        if(!reporter_) {
          reporter_ = new DiagnosticsReporter(state);
        }
      }

      void report(DiagnosticsData* data) {
        if(reporter_) {
          reporter_->report(data);
        }
      }

      void add_measurement(Measurement* m) {
        measurements_.insert(m);
      }
    };
  }
}

#endif
