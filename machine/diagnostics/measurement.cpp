#include "configuration.hpp"
#include "environment.hpp"
#include "shared_state.hpp"
#include "state.hpp"
#include "vm.hpp"
#include "call_frame.hpp"
#include "diagnostics.hpp"

#include "class/compiled_code.hpp"

#include "diagnostics/measurement.hpp"

#include <ostream>

namespace rubinius {
  namespace diagnostics {
    Measurement::Measurement(STATE, CompiledCode* code, int ip)
      : label()
    {
      std::ostringstream l;

      l << code->name()->cpp_str(state) << " at "
        << code->file()->cpp_str(state) << ":" << code->start_line(state)
        << "(+" << ip << ")";

      label = l.str();

      set_type("Measurement");

      rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

      document_.AddMember("label", rapidjson::Value(label.c_str(), label.size()).Move(), alloc);
    }

    Counter::Counter(STATE, CompiledCode* code, int ip)
      : Measurement(state, code, ip)
      , value(0)
    {
      set_type("Counter");

      rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

      document_.AddMember("value", rapidjson::Value().SetInt(value.load()), alloc);
    }
  }
}
