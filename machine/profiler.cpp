#include "diagnostics.hpp"
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
  namespace diagnostics {
    ProfilerDiagnostics::ProfilerDiagnostics()
    {
      /* TODO: diagnostics
      set_type("Profiler");

      document.AddMember("thread", 0, document.GetAllocator());
      document.AddMember("samples", 0, document.GetAllocator());
      document.AddMember("sample_average_time", 0.0, document.GetAllocator());
      document.AddMember("total_time", 0.0, document.GetAllocator());
      document.AddMember("entries", 0, document.GetAllocator());
      */
    }
  }

  namespace profiler {
    Profiler::Profiler(STATE)
      : path_()
      , diagnostics_data_(NULL)
    {
    }

    void Profiler::report(STATE) {
    }
  }
}
