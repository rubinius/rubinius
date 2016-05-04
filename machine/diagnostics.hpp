#ifndef RBX_DIAGNOSTICS_HPP
#define RBX_DIAGNOSTICS_HPP

#include "internal_threads.hpp"

#include "util/thread.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#include <stdint.h>
#include <list>

namespace rubinius {
  namespace diagnostics {
    class DiagnosticsData {
      rapidjson::Document* document_;

    public:
      DiagnosticsData()
        : document_(NULL)
      { }
      virtual ~DiagnosticsData();

      void to_string(rapidjson::StringBuffer buffer);
    };

    class MemoryDiagnostics : public DiagnosticsData {
    public:
      int64_t objects_;
      int64_t bytes_;

      MemoryDiagnostics()
        : DiagnosticsData()
        , objects_(0)
        , bytes_(0)
      { }
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

    class Diagnostics : public InternalThread {
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
