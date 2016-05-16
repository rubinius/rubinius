#ifndef RBX_PROFILER_H
#define RBX_PROFILER_H

namespace rubinius {
  namespace profiler {
    class ProfilerDiagnostics : public diagnostics::DiagnosticsData {
      ProfilerDiagnostics();
      virtual ~ProfilerDiagnostics() { }
    };

    enum ProfilerTarget {
      eNone,
      ePath,
      eDiagnostics
    };

    class Profiler {
      std::string path_;
      ProfilerTarget target_;
      bool report_;

    public:

    void report_profile(STATE);
    void report_profile_file(STATE, Tuple* profile, double total_time);
    void report_profile_diagnostics(STATE, Tuple* profile, double total_time);

    std::string& profiler_path() {
      return profiler_path_;
    }

    bool profiler_enabled_p() {
      return profiler_enabled_;
    }

    ProfilerTarget profiler_target() {
      return profiler_target_;
    }
    void set_profiler_path();
      void after_fork_child(STATE);
    };
  }
}

#endif
