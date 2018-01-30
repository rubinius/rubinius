#ifndef RBX_PROFILER_H
#define RBX_PROFILER_H

#include "defines.hpp"

#include "diagnostics/profiler.hpp"

namespace rubinius {
  namespace profiler {
    class Profiler {
      std::string path_;
      diagnostics::ProfilerDiagnostics* diagnostics_data_;

    public:
      Profiler(STATE);

      virtual ~Profiler() {
        if(diagnostics_data_) delete diagnostics_data_;
      }

      diagnostics::ProfilerDiagnostics* diagnostics_data() {
        if(!diagnostics_data_) {
          diagnostics_data_ = new diagnostics::ProfilerDiagnostics();
        }

        return diagnostics_data_;
      }

      void report(STATE);
    };
  }
}

#endif
