#include "system_diagnostics.hpp"

namespace rubinius {
  namespace diagnostics {
    void SystemDiagnostics::log() {
      memory_.log();
    }
  }
}
