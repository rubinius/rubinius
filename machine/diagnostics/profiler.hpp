#ifndef RBX_DIAGNOSTICS_PROFILER_HPP
#define RBX_DIAGNOSTICS_PROFILER_HPP

namespace rubinius {
  namespace diagnostics {
    class ProfilerDiagnostics {
    public:
      ProfilerDiagnostics();
      virtual ~ProfilerDiagnostics() { }
    };
  }
}

#endif
