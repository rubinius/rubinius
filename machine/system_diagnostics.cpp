#include "system_diagnostics.hpp"

namespace rubinius {
  namespace diagnostics {
    void ObjectDiagnostics::log() {
      bool summarize = immix_.modified_p() ||
        mark_sweep_.modified_p() || headers_.modified_p() ||
        handles_.modified_p() || code_.modified_p() || symbols_.modified_p();

      immix_.log();
      mark_sweep_.log();
      headers_.log();
      handles_.log();
      code_.log();
      symbols_.log();

      if(summarize) {
        logger::write("object memory: diagnostics: total memory: %ld",
            immix_.total_bytes_ +
            mark_sweep_.bytes_ +
            headers_.bytes_ +
            handles_.bytes_ +
            code_.bytes_ +
            symbols_.bytes_);
      }
    }

    void SystemDiagnostics::log() {
      memory_->log();
    }
  }
}
