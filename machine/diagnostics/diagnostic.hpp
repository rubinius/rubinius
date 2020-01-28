#ifndef RBX_DIAGNOSTICS_DIAGNOSTIC_HPP
#define RBX_DIAGNOSTICS_DIAGNOSTIC_HPP

#include "configuration.hpp"
#include "environment.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <chrono>
#include <ctime>
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
        state->machine()->diagnostics()->add_report(state, this);
        set_nodename(state->environment()->nodename());
      }

      virtual void stop_reporting(STATE) {
        state->machine()->diagnostics()->remove_report(this);
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

#define RBX_DIAGNOSTICS_TIME_LEN    100

      virtual void set_timestamp() {
        auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        char st[RBX_DIAGNOSTICS_TIME_LEN];
        size_t len = std::strftime(st, RBX_DIAGNOSTICS_TIME_LEN, "%c", std::localtime(&tt));

        document_["metadata"]["timestamp"].SetString(rapidjson::StringRef(st, len), alloc);
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
