#ifndef RBX_DIAGNOSTICS_HPP
#define RBX_DIAGNOSTICS_HPP

#include "machine_threads.hpp"

#include "util/thread.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#include <stdint.h>
#include <list>

namespace rubinius {
  namespace diagnostics {
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

    class Diagnostics : public MachineThread {
      DiagnosticsList list_;

      DiagnosticsEmitter* emitter_;

      utilities::thread::Mutex diagnostics_lock_;
      utilities::thread::Condition diagnostics_condition_;

    public:
      Diagnostics(STATE);
      virtual ~Diagnostics() { }

      void report(DiagnosticsData* data);

      void initialize(STATE);
      void run(STATE);
      void wakeup(STATE);
      void after_fork_child(STATE);
    };
  }
}

#endif
