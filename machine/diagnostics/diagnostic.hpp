#ifndef RBX_DIAGNOSTICS_DIAGNOSTIC_HPP
#define RBX_DIAGNOSTICS_DIAGNOSTIC_HPP

#include "configuration.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <chrono>
#include <iomanip>
#include <ostream>
#include <string>
#include <utility>

namespace rubinius {
  namespace diagnostics {
    class Diagnostic {
    protected:
      rapidjson::Document document_;
      int sequence;

    public:
      Diagnostic()
        : document_()
        , sequence(0)
      {
        document_.SetObject();

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        rapidjson::Value o(rapidjson::kObjectType);
        o.AddMember("type", "Diagnostics", alloc);
        o.AddMember("nodename", "", alloc);
        o.AddMember("timestamp", sequence, alloc);
        o.AddMember("sequence", sequence, alloc);

        document_.AddMember("metadata", o.Move(), alloc);
      }

      virtual ~Diagnostic() {}

      virtual void start_reporting(STATE) {
        state->shared().diagnostics()->add_report(state, this);
        set_nodename(state->shared().nodename);
      }

      virtual void stop_reporting(STATE) {
        state->shared().diagnostics()->remove_report(this);
      }

      virtual void update() {
        set_timestamp();
        set_sequence();
      }

      virtual void set_type(const char* type) {
        document_["metadata"]["type"].SetString(type, document_.GetAllocator());
      }

      virtual void set_nodename(const std::string& node) {
        document_["metadata"]["nodename"].SetString(
            node.c_str(), node.size(), document_.GetAllocator());
      }

      virtual void set_timestamp() {
        auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::ostringstream t;
        t << std::put_time(std::localtime(&tt), "%c");

        document_["metadata"]["timestamp"].SetString(
            t.str().c_str(), t.str().size(), document_.GetAllocator());
      }

      virtual void set_sequence() {
        document_["metadata"]["sequence"] = ++sequence;
      }

      virtual const std::string to_string() {
        rapidjson::StringBuffer buffer;

        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        document_.Accept(writer);

        return std::move(buffer.GetString());
      }
    };
  }
}

#endif
