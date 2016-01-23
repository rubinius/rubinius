#ifndef RBX_SYSTEM_DIAGNOSTICS_HPP
#define RBX_SYSTEM_DIAGNOSTICS_HPP

#include "diagnostics.hpp"
#include "object_memory.hpp"
#include "symbol_table.hpp"

#include "capi/handles.hpp"

#include "gc/code_manager.hpp"
#include "gc/immix.hpp"
#include "gc/inflated_headers.hpp"
#include "gc/mark_sweep.hpp"

namespace rubinius {
  namespace diagnostics {
    class ObjectDiagnostics {
      rubinius::ImmixGC::Diagnostics& immix_;
      rubinius::MarkSweepGC::Diagnostics& mark_sweep_;
      rubinius::InflatedHeaders::Diagnostics& headers_;
      rubinius::capi::Handles::Diagnostics& handles_;
      rubinius::CodeManager::Diagnostics& code_;
      rubinius::SymbolTable::Diagnostics& symbols_;

    public:
      ObjectDiagnostics(rubinius::ImmixGC::Diagnostics& immix,
          rubinius::MarkSweepGC::Diagnostics& mark_sweep,
          rubinius::InflatedHeaders::Diagnostics& inflated_headers,
          rubinius::capi::Handles::Diagnostics& capi_handles,
          rubinius::CodeManager::Diagnostics& code_manager,
          rubinius::SymbolTable::Diagnostics& symbols)
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
