#ifndef RBX_DIAGNOSTICS_DIAGNOSTIC_HPP
#define RBX_DIAGNOSTICS_DIAGNOSTIC_HPP

#include "configuration.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <utility>
#include <string>

namespace rubinius {
  namespace diagnostics {
    class Diagnostic {
    protected:
      rapidjson::Document document_;

    public:
      Diagnostic() {
        document_.SetObject();
        document_.AddMember("type", "Diagnostics", document_.GetAllocator());
      }

      virtual ~Diagnostic() {}

      virtual void start_reporting(STATE) {
        state->shared().diagnostics()->add_report(state, this);
      }

      virtual void stop_reporting(STATE) {
        state->shared().diagnostics()->remove_report(this);
      }

      virtual void update() {}

      virtual void set_type(const char* type) {
        document_["type"].SetString(type, document_.GetAllocator());
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
