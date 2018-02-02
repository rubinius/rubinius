#ifndef RBX_DIAGNOSTICS_MEMORY_HPP
#define RBX_DIAGNOSTICS_MEMORY_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class Memory : public Diagnostic {
    public:
      uint64_t objects_;
      uint64_t bytes_;
      int64_t collections_;

      Memory()
        : Diagnostic()
        , objects_(0)
        , bytes_(0)
        , collections_(0)
      {
        set_type("Memory");

        document_.AddMember("objects", objects_, document_.GetAllocator());
        document_.AddMember("bytes", bytes_, document_.GetAllocator());
        document_.AddMember("collections", collections_, document_.GetAllocator());
      }

      virtual ~Memory() { }

      virtual void update() {
        document_["objects"] = objects_;
        document_["bytes"] = bytes_;
        document_["collections"] = collections_;
      }
    };

    class CodeManager : public Memory {
    public:
      int64_t chunks_;

      CodeManager()
        : Memory()
        , chunks_(0)
      {
        set_type("CodeManager");

        document_.AddMember("chunks", chunks_, document_.GetAllocator());
      }

      virtual void update() {
        document_["chunks"] = chunks_;
      }
    };

    class Handles : public Memory {
    public:

      Handles()
        : Memory()
      {
        set_type("CAPIHandles");
      }
    };

    class Immix : public Memory {
    public:
      int64_t total_bytes_;
      int64_t chunks_;
      int64_t holes_;
      double percentage_;

      Immix()
        : Memory()
        , total_bytes_(0)
        , chunks_(0)
        , holes_(0)
        , percentage_(0.0)
      {
        set_type("ImmixCollector");

        document_.AddMember("total_bytes", total_bytes_, document_.GetAllocator());
        document_.AddMember("chunks", chunks_, document_.GetAllocator());
        document_.AddMember("holes", holes_, document_.GetAllocator());
        document_.AddMember("percentage", percentage_, document_.GetAllocator());
      }

      virtual void update() {
        Memory::update();

        document_["total_bytes"] = total_bytes_;
        document_["chunks"] = chunks_;
        document_["holes"] = holes_;
        document_["percentage"] = percentage_;
      }
    };

    class InflatedHeader : public Memory {
    public:

      InflatedHeader()
        : Memory()
      {
        set_type("InflatedHeaders");
      }
    };

    class MarkSweep : public Memory {
    public:

      MarkSweep()
        : Memory()
      {
        set_type("MarkSweepCollector");
      }
    };

    class SymbolTable : public Memory {
    public:
      int64_t symbols_;

      SymbolTable()
        : Memory()
        , symbols_(0)
      {
        set_type("SymbolTable");

        document_.AddMember("symbols", symbols_, document_.GetAllocator());
      }

      virtual void update() {
        Memory::update();

        document_["symbols"] = symbols_;
      }
    };

    class MemoryMetrics {
    public:
      metric young_bytes;
      metric young_objects;
      metric immix_bytes;
      metric immix_objects;
      metric immix_chunks;
      metric large_bytes;
      metric large_objects;
      metric symbols;
      metric symbols_bytes;
      metric code_bytes;
      metric jit_bytes;
      metric promoted_bytes;
      metric promoted_objects;
      metric slab_refills;
      metric slab_refills_fails;
      metric data_objects;
      metric capi_handles;
      metric inflated_headers;
      metric suspend_ms;
      metric allocated_bytes;
      metric malloc;
      metric calloc;
      metric realloc;
      metric freed;

      MemoryMetrics() {
        young_bytes = 0;
        young_objects = 0;
        immix_bytes = 0;
        immix_objects = 0;
        immix_chunks = 0;
        large_bytes = 0;
        large_objects = 0;
        symbols = 0;
        symbols_bytes = 0;
        code_bytes = 0;
        jit_bytes = 0;
        promoted_bytes = 0;
        promoted_objects = 0;
        slab_refills = 0;
        slab_refills_fails = 0;
        data_objects = 0;
        capi_handles = 0;
        inflated_headers = 0;
        suspend_ms = 0;
        allocated_bytes = 0;
        malloc = 0;
        calloc = 0;
        realloc = 0;
        freed = 0;
      }
    };
  }
}

#endif
