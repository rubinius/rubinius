#ifndef RBX_DIAGNOSTICS_JIT_HPP
#define RBX_DIAGNOSTICS_JIT_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class JITMetrics : public Diagnostic {
    public:
      metric methods_queued;
      metric methods_compiled;
      metric methods_failed;
      metric compile_time_us;
      metric uncommon_exits;
      metric inlined_accessors;
      metric inlined_methods;
      metric inlined_blocks;
      metric inlined_primitives;
      metric inlined_ffi;

      JITMetrics()
        : Diagnostic()
        , methods_queued(0)
        , methods_compiled(0)
        , methods_failed(0)
        , compile_time_us(0)
        , uncommon_exits(0)
        , inlined_accessors(0)
        , inlined_methods(0)
        , inlined_blocks(0)
        , inlined_primitives(0)
        , inlined_ffi(0)
      {
        set_type("JITMetrics");

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_.AddMember("methods_queued", methods_queued, alloc);
        document_.AddMember("methods_compiled", methods_compiled, alloc);
        document_.AddMember("methods_failed", methods_failed, alloc);
        document_.AddMember("compile_time_us", compile_time_us, alloc);
        document_.AddMember("uncommon_exits", uncommon_exits, alloc);
        document_.AddMember("inlined_accessors", inlined_accessors, alloc);
        document_.AddMember("inlined_methods", inlined_methods, alloc);
        document_.AddMember("inlined_blocks", inlined_blocks, alloc);
        document_.AddMember("inlined_primitives", inlined_primitives, alloc);
        document_.AddMember("inlined_ffi", inlined_ffi, alloc);
      }

      virtual ~JITMetrics() { }

      virtual void update() {
        document_["methods_queued"] = methods_queued;
        document_["methods_compiled"] = methods_compiled;
        document_["methods_failed"] = methods_failed;
        document_["compile_time_us"] = compile_time_us;
        document_["uncommon_exits"] = uncommon_exits;
        document_["inlined_accessors"] = inlined_accessors;
        document_["inlined_methods"] = inlined_methods;
        document_["inlined_blocks"] = inlined_blocks;
        document_["inlined_primitives"] = inlined_primitives;
        document_["inlined_ffi"] = inlined_ffi;
      }

      virtual void start_reporting(STATE) {
        if(state->configuration()->diagnostics_jit_enabled) {
          Diagnostic::start_reporting(state);
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->configuration()->diagnostics_jit_enabled) {
          Diagnostic::stop_reporting(state);
        }
      }
    };
  }
}

#endif
