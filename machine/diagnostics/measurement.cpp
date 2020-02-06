#include "configuration.hpp"
#include "environment.hpp"
#include "thread_state.hpp"
#include "call_frame.hpp"
#include "diagnostics.hpp"

#include "class/compiled_code.hpp"

#include "diagnostics/measurement.hpp"

#include <sstream>

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

    Bytes::Bytes(STATE, CompiledCode* code, int ip)
      : Measurement(state, code, ip)
      , bytes()
    {
      set_type("Bytes");

      rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

      document_.AddMember("value", rapidjson::Value().SetInt(bytes.load()), alloc);
    }

    Counter::Counter(STATE, CompiledCode* code, int ip)
      : Measurement(state, code, ip)
      , counter(0)
    {
      set_type("Counter");

      rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

      document_.AddMember("value", rapidjson::Value().SetInt(counter.load()), alloc);
    }

    Sum::Sum(STATE, CompiledCode* code, int ip)
      : Measurement(state, code, ip)
      , sum(0)
    {
      set_type("Sum");

      rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

      document_.AddMember("value", rapidjson::Value().SetInt(sum.load()), alloc);
    }

    TimeStamp::TimeStamp(STATE, CompiledCode* code, int ip)
      : Measurement(state, code, ip)
      , time_stamp()
    {
      set_type("TimeStamp");

      rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

      document_.AddMember("value", "", alloc);
    }

    Timer::Timer(STATE, CompiledCode* code, int ip)
      : Measurement(state, code, ip)
      , lock()
      , total()
      , begin()
    {
      set_type("Timer");

      rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

      document_.AddMember("value", "", alloc);
    }

    Value::Value(STATE, CompiledCode* code, int ip)
      : Measurement(state, code, ip)
      , lock()
      , value()
    {
      set_type("Value");

      rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

      document_.AddMember("value", "", alloc);
    }
  }
}
