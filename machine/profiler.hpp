#ifndef RBX_PROFILER_H
#define RBX_PROFILER_H

#include "defines.hpp"
#include "diagnostics.hpp"

namespace rubinius {
  namespace profiler {
    class ProfilerDiagnostics : public diagnostics::DiagnosticsData {
    public:
      ProfilerDiagnostics();
      virtual ~ProfilerDiagnostics() { }
    };

    class Profiler {
      std::string path_;
      ProfilerDiagnostics* diagnostics_data_;

    public:
      Profiler(STATE);

      virtual ~Profiler() {
        if(diagnostics_data_) delete diagnostics_data_;
      }

      ProfilerDiagnostics* diagnostics_data() {
        if(!diagnostics_data_) {
          diagnostics_data_ = new ProfilerDiagnostics();
        }

        return diagnostics_data_;
      }

      void report(STATE);
    };
  }
}

#endif
