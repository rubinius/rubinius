#include "profiler.hpp"

namespace rubinius {
  namespace profiler {

    void SharedState::set_profiler_path() {
      profiler_path_ = config.system_profiler_target.value;
      env()->expand_config_value(profiler_path_, "$PID", pid.c_str());
    }

    Profiler::Profiler(STATE)
    {
      profiler_enabled_ = true;

      if(!config.system_profiler_target.value.compare("none")) {
        profiler_enabled_ = false;
      } else if(!config.system_profiler_target.value.compare("diagnostics")) {
        profiler_target_ = eDiagnostics;
      } else {
        profiler_target_ = ePath;
        set_profiler_path();
      }
    }

    void Profiler::after_fork_child(STATE) {
      if(profiler_enabled_p()) {
        if(config.system_profiler_subprocess.value) {
          set_profiler_path();
        } else {
          profiler_enabled_ = false;
        }
      }
    }

    void VM::report_profile_file(STATE, Tuple* profile, double total_time) {
      std::ofstream file;
      file.open(state->shared().profiler_path(), std::fstream::out | std::fstream::app);

      file << "Profile: thread: " << thread_id()
        << ", samples: " << profile_sample_count_
        << ", sample avg time: " << (total_time / profile_sample_count_)
        << ", total time: " << total_time << "s" << std::endl;

      ::qsort(reinterpret_cast<void*>(profile->field), profile->num_fields(),
          sizeof(intptr_t), profile_compare);

      file << std::endl
        << std::setw(5) << "%"
        << std::setw(10) << "Samples"
        << "  Method"
        << std::endl
        << "------------------------------------------------------------"
        << std::endl;

      for(native_int i = 0; i < profile->num_fields(); i++) {
        if(CompiledCode* code = try_as<CompiledCode>(profile->at(i))) {
          file << std::setw(5) << std::setprecision(1) << std::fixed
            << ((double)code->machine_code()->sample_count / profile_sample_count_ * 100)
            << std::setw(10)
            << code->machine_code()->sample_count
            << "  "
            << *code->machine_code()->description()
            << std::endl;
        }
      }

      file << std::endl;
      file.close();
    }

    void VM::report_profile_diagnostics(STATE, Tuple* profile, double total_time) {

    }

    void VM::report_profile(STATE) {
      if(!state->shared().profiler_enabled_p()) return;

      Tuple* profile = profile_.get();
      if(profile->nil_p()) return;

      double total_time = run_time();

      switch(state->shared().profiler_target()) {
      case SharedState::ePath:
        report_profile_file(state, profile, total_time);
        break;
      case SharedState::eDiagnostics:
        report_profile_diagnostics(state, profile, total_time);
        break;
      default:
        return;
      }
    }
    
  }
}
