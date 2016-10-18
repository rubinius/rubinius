#include "environment.hpp"
#include "machine_code.hpp"
#include "profiler.hpp"

#include "class/compiled_code.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>

namespace rubinius {
  namespace profiler {
    ProfilerDiagnostics::ProfilerDiagnostics()
      : diagnostics::DiagnosticsData()
    {
      set_type("Profiler");

      document.AddMember("thread", 0, document.GetAllocator());
      document.AddMember("samples", 0, document.GetAllocator());
      document.AddMember("sample_average_time", 0.0, document.GetAllocator());
      document.AddMember("total_time", 0.0, document.GetAllocator());
      document.AddMember("entries", 0, document.GetAllocator());
    }

    Profiler::Profiler(STATE)
      : path_()
      , target_(eNone)
      , diagnostics_data_(NULL)
    {
      std::string& target = state->shared().config.system_profiler_target.value;

      if(!target.compare("none")) {
        // target_(eNone)
      } else if(!target.compare("diagnostics")) {
        target_ = eDiagnostics;
      } else {
        target_ = ePath;
        set_profiler_path(state);
      }
    }

    void Profiler::set_profiler_path(STATE) {
      path_ = state->shared().config.system_profiler_target.value;
      state->shared().env()->expand_config_value(
          path_, "$PID", state->shared().pid.c_str());
    }

    void Profiler::after_fork_child(STATE) {
      if(target_ != eNone) {
        if(state->shared().config.system_profiler_subprocess.value) {
          set_profiler_path(state);
        } else {
          target_ = eNone;
        }
      }
    }

    void Profiler::report(STATE) {
      switch(target_) {
      case ePath:
        report_to_file(state);
        break;
      case eDiagnostics:
        report_to_diagnostics(state);
        break;
      default:
        return;
      }
    }

    void Profiler::report_to_file(STATE) {
      state->vm()->sort_profile();
      CompiledCode** profile = state->vm()->profile();

      double total_time = state->vm()->run_time();

      std::ofstream file;
      file.open(path_, std::fstream::out | std::fstream::app);

      file << "Profile: thread: " << state->vm()->thread_id()
        << ", samples: " << state->vm()->profile_sample_count()
        << ", sample avg time: " << (total_time / state->vm()->profile_sample_count()) << "s"
        << ", total time: " << total_time << "s" << std::endl;

      state->vm()->sort_profile();

      file << std::endl
        << std::setw(5) << "%"
        << std::setw(10) << "Samples"
        << "  Method"
        << std::endl
        << "------------------------------------------------------------"
        << std::endl;

      for(native_int i = 0; i < state->vm()->max_profile_entries(); i++) {
        if(CompiledCode* code = try_as<CompiledCode>(profile[i])) {
          double percentage = (double)code->machine_code()->sample_count
            / state->vm()->profile_sample_count() * 100;

          file << std::setw(5) << std::setprecision(1) << std::fixed
            << percentage
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

    void Profiler::report_to_diagnostics(STATE) {
      ProfilerDiagnostics* data = diagnostics_data();

      state->vm()->sort_profile();
      CompiledCode** profile = state->vm()->profile();

      double total_time = state->vm()->run_time();
      double sample_avg = total_time / state->vm()->profile_sample_count();

      data->document["thread"] = state->vm()->thread_id();
      data->document["samples"] = state->vm()->profile_sample_count();
      data->document["sample_average_time"] = sample_avg;
      data->document["total_time"] = total_time;

      rapidjson::Value& entries = data->document["entries"];
      entries.SetArray();

      rapidjson::Document::AllocatorType& allocator = data->document.GetAllocator();

      for(native_int i = 0; i < state->vm()->max_profile_entries(); i++) {
        if(CompiledCode* code = try_as<CompiledCode>(profile[i])) {
          rapidjson::Value percentage(
              (double)code->machine_code()->sample_count
              / state->vm()->profile_sample_count() * 100);

          rapidjson::Value samples((uint64_t)code->machine_code()->sample_count);

          rapidjson::Value description;
          description.SetString(code->machine_code()->description()->c_str(), allocator);

          rapidjson::Value row = rapidjson::Value();
          row.SetArray();

          row.PushBack(percentage, allocator);
          row.PushBack(samples, allocator);
          row.PushBack(description, allocator);

          data->document["entries"].PushBack(row, allocator);
        }
      }

      state->shared().report_diagnostics(data);
    }
  }
}
