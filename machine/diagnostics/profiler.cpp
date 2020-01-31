#include "configuration.hpp"
#include "environment.hpp"
#include "thread_state.hpp"

#include "diagnostics/profiler.hpp"

namespace rubinius {
  namespace diagnostics {
    bool operator==(const ProfilerEntryKey& a, const ProfilerEntryKey& b) {
      return a.serial == b.serial && a.ip == b.ip;
    }
  }
}
