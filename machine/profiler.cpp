#include "environment.hpp"
#include "machine_code.hpp"
#include "profiler.hpp"

#include "class/compiled_code.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

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
      , diagnostics_data_(NULL)
    {
    }

    void Profiler::report(STATE) {
    }

    static bool profile_cmp(const CompiledCode* left, const CompiledCode* right) {
      return left->machine_code()->sample_count > right->machine_code()->sample_count;
    }
  }
}
