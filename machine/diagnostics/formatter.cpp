#include "diagnostics/codedb.hpp"
#include "diagnostics/concurrency.hpp"
#include "diagnostics/console.hpp"
#include "diagnostics/gc.hpp"
#include "diagnostics/jit.hpp"
#include "diagnostics/machine.hpp"
#include "diagnostics/memory.hpp"
#include "diagnostics/profiler.hpp"

#include "diagnostics/formatter.hpp"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <utility>

namespace rubinius {
  namespace diagnostics {
    ImmixFormatter::ImmixFormatter(ImmixDiagnostics* d)
      : Formatter()
      , diagnostics_(d)
    {
      set_type("ImmixCollector");

      document_.AddMember("objects", diagnostics_->objects_, document_.GetAllocator());
      document_.AddMember("bytes", diagnostics_->bytes_, document_.GetAllocator());
      document_.AddMember("collections", diagnostics_->collections_, document_.GetAllocator());
      document_.AddMember("total_bytes", diagnostics_->total_bytes_, document_.GetAllocator());
      document_.AddMember("chunks", diagnostics_->chunks_, document_.GetAllocator());
      document_.AddMember("holes", diagnostics_->holes_, document_.GetAllocator());
      document_.AddMember("percentage", diagnostics_->percentage_, document_.GetAllocator());
    }

    void ImmixFormatter::update() {
      document_["objects"] = diagnostics_->objects_;
      document_["bytes"] = diagnostics_->bytes_;
      document_["collections"] = diagnostics_->collections_;
      document_["total_bytes"] = diagnostics_->total_bytes_;
      document_["chunks"] = diagnostics_->chunks_;
      document_["holes"] = diagnostics_->holes_;
      document_["percentage"] = diagnostics_->percentage_;
    }

    std::string ImmixFormatter::to_string() {
      rapidjson::StringBuffer buffer;

      rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

      document_.Accept(writer);

      return std::move(buffer.GetString());
    }
  }
}
