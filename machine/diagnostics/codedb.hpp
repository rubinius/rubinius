#ifndef RBX_DIAGNOSTICS_CODEDB_HPP
#define RBX_DIAGNOSTICS_CODEDB_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class CodeDBMetrics {
    public:
      metric load_ns;
      metric load_count;

      CodeDBMetrics() {
        load_ns = 0;
        load_count = 0;
      }
    };
  }
}

#endif
