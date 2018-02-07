#ifndef RBX_DIAGNOSTICS_MEASUREMENT_HPP
#define RBX_DIAGNOSTICS_MEASUREMENT_HPP

#include "diagnostics/diagnostic.hpp"

#include <atomic>
#include <string>

namespace rubinius {
  class CompiledCode;

  namespace diagnostics {
    class Measurement : public Diagnostic {
    public:
      std::string label;

      Measurement(STATE, CompiledCode* code, int ip);

      virtual ~Measurement() { }
    };

    class Counter : public Measurement {
    public:
      std::atomic<intptr_t> value;

      Counter(STATE, CompiledCode* code, int ip);

      virtual ~Counter() { }

      void update(STATE, intptr_t v) {
        value.fetch_add(v);
      }

      virtual void update() {
        document_["value"] = rapidjson::Value().SetInt(value.load());
      }
    };
  }
}

#endif
