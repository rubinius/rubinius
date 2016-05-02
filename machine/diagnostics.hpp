#ifndef RBX_DIAGNOSTICS_HPP
#define RBX_DIAGNOSTICS_HPP

#include "internal_threads.hpp"

#include "util/thread.hpp"

#include <stdint.h>
#include <list>

namespace rubinius {
  namespace diagnostics {
    class DiagnosticsData {
      bool modified_;

    public:
      DiagnosticsData() : modified_(false) { }

      bool modified_p() {
        return modified_;
      }

      void modify() {
        modified_ = true;
      }

      void log() {
      }
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

    typedef std::list<DiagnosticsData*> DiagnosticsList;

    class Diagnostics : public InternalThread {
      DiagnosticsList list_;

      utilities::thread::Mutex diagnostics_lock_;
      utilities::thread::Condition diagnostics_condition_;

    public:
      Diagnostics(STATE);
      virtual ~Diagnostics() { }

      void initialize(STATE);
      void run(STATE);
      void wakeup(STATE);
      void after_fork_child(STATE);
    };
  }
}

#endif
