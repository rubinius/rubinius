#ifndef RBX_SYSTEM_DIAGNOSTICS_HPP
#define RBX_SYSTEM_DIAGNOSTICS_HPP

#include "diagnostics.hpp"
#include "object_memory.hpp"
#include "symbol_table.hpp"

#include "capi/handles.hpp"

#include "memory/code_manager.hpp"
#include "memory/immix_collector.hpp"
#include "memory/inflated_headers.hpp"
#include "memory/mark_sweep.hpp"

namespace rubinius {
  namespace diagnostics {
    class ObjectDiagnostics {
      memory::ImmixGC::Diagnostics& immix_;
      memory::MarkSweepGC::Diagnostics& mark_sweep_;
      memory::InflatedHeaders::Diagnostics& headers_;
      capi::Handles::Diagnostics& handles_;
      memory::CodeManager::Diagnostics& code_;
      SymbolTable::Diagnostics& symbols_;

    public:
      ObjectDiagnostics(memory::ImmixGC::Diagnostics& immix,
          memory::MarkSweepGC::Diagnostics& mark_sweep,
          memory::InflatedHeaders::Diagnostics& inflated_headers,
          capi::Handles::Diagnostics& capi_handles,
          memory::CodeManager::Diagnostics& code_manager,
          SymbolTable::Diagnostics& symbols)
        : immix_(immix)
        , mark_sweep_(mark_sweep)
        , headers_(inflated_headers)
        , handles_(capi_handles)
        , code_(code_manager)
        , symbols_(symbols)
      { }

      void log();
    };

    class SystemDiagnostics {
      ObjectDiagnostics* memory_;

    public:
      SystemDiagnostics(ObjectDiagnostics* memory)
        : memory_(memory)
      { }

      void log();
    };
  }
}

#endif
