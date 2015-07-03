#ifndef RBX_SYSTEM_DIAGNOSTICS_HPP
#define RBX_SYSTEM_DIAGNOSTICS_HPP

#include "diagnostics.hpp"
#include "object_memory.hpp"

namespace rubinius {
  namespace diagnostics {
    class SystemDiagnostics {
      rubinius::ObjectMemory::Diagnostics& memory_;

    public:
      SystemDiagnostics(rubinius::ObjectMemory::Diagnostics& memory)
        : memory_(memory)
      { }

      void log();
    };
  }
}

#endif
