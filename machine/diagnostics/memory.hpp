#ifndef RBX_DIAGNOSTICS_MEMORY_HPP
#define RBX_DIAGNOSTICS_MEMORY_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class Memory : public Diagnostic {
    public:
      uint64_t objects;
      uint64_t bytes;
      int64_t collections;

      Memory()
        : Diagnostic()
        , objects(0)
        , bytes(0)
        , collections(0)
      {
        set_type("Memory");

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_.AddMember("objects", objects, alloc);
        document_.AddMember("bytes", bytes, alloc);
        document_.AddMember("collections", collections, alloc);
      }

      virtual ~Memory() { }

      virtual void update() {
        Diagnostic::update();

        document_["objects"] = objects;
        document_["bytes"] = bytes;
        document_["collections"] = collections;
      }
    };

    class CodeManager : public Memory {
    public:
      int64_t chunks;

      CodeManager()
        : Memory()
        , chunks(0)
      {
        set_type("CodeManager");

        document_.AddMember("chunks", chunks, document_.GetAllocator());
      }

      virtual void update() {
        Diagnostic::update();

        document_["chunks"] = chunks;
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
      int64_t total_bytes;
      int64_t chunks;
      int64_t holes;
      double percentage;

      Immix()
        : Memory()
        , total_bytes(0)
        , chunks(0)
        , holes(0)
        , percentage(0.0)
      {
        set_type("ImmixCollector");

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_.AddMember("total_bytes", total_bytes, alloc);
        document_.AddMember("chunks", chunks, alloc);
        document_.AddMember("holes", holes, alloc);
        document_.AddMember("percentage", percentage, alloc);
      }

      virtual void update() {
        Memory::update();

        document_["total_bytes"] = total_bytes;
        document_["chunks"] = chunks;
        document_["holes"] = holes;
        document_["percentage"] = percentage;
      }
    };

    class ExtendedHeader : public Memory {
    public:

      ExtendedHeader()
        : Memory()
      {
        set_type("ExtendedHeaders");
      }
    };

    class LargeRegion : public Memory {
    public:

      LargeRegion()
        : Memory()
      {
        set_type("LargeRegion");
      }
    };

    class SymbolTable : public Memory {
    public:

      SymbolTable()
        : Memory()
      {
        set_type("SymbolTable");
      }
    };

    class MemoryMetrics : public Diagnostic {
    public:
      metric first_region_bytes;
      metric first_region_objects;
      metric first_region_chunks;
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

      MemoryMetrics()
        : Diagnostic()
        , first_region_bytes(0)
        , first_region_objects(0)
        , first_region_chunks(0)
        , large_bytes(0)
        , large_objects(0)
        , symbols(0)
        , symbols_bytes(0)
        , code_bytes(0)
        , jit_bytes(0)
        , promoted_bytes(0)
        , promoted_objects(0)
        , slab_refills(0)
        , slab_refills_fails(0)
        , data_objects(0)
        , capi_handles(0)
        , inflated_headers(0)
        , suspend_ms(0)
        , allocated_bytes(0)
        , malloc(0)
        , calloc(0)
        , realloc(0)
        , freed(0)
      {
        set_type("MemoryMetrics");

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_.AddMember("first_region_bytes", first_region_bytes, alloc);
        document_.AddMember("first_region_objects", first_region_objects, alloc);
        document_.AddMember("first_region_chunks", first_region_chunks, alloc);
        document_.AddMember("large_bytes", large_bytes, alloc);
        document_.AddMember("large_objects", large_objects, alloc);
        document_.AddMember("symbols", symbols, alloc);
        document_.AddMember("symbols_bytes", symbols_bytes, alloc);
        document_.AddMember("code_bytes", code_bytes, alloc);
        document_.AddMember("jit_bytes", jit_bytes, alloc);
        document_.AddMember("promoted_bytes", promoted_bytes, alloc);
        document_.AddMember("promoted_objects", promoted_objects, alloc);
        document_.AddMember("slab_refills", slab_refills, alloc);
        document_.AddMember("slab_refills_fails", slab_refills_fails, alloc);
        document_.AddMember("data_objects", data_objects, alloc);
        document_.AddMember("capi_handles", capi_handles, alloc);
        document_.AddMember("inflated_headers", inflated_headers, alloc);
        document_.AddMember("suspend.ms", suspend_ms, alloc);
        document_.AddMember("allocated_bytes", allocated_bytes, alloc);
        document_.AddMember("malloc", malloc, alloc);
        document_.AddMember("calloc", calloc, alloc);
        document_.AddMember("realloc", realloc, alloc);
        document_.AddMember("freed", freed, alloc);
      }

      virtual void update() {
        Diagnostic::update();

        document_["first_region_bytes"] = first_region_bytes;
        document_["first_region_objects"] = first_region_objects;
        document_["first_region_chunks"] = first_region_chunks;
        document_["large_bytes"] = large_bytes;
        document_["large_objects"] = large_objects;
        document_["symbols"] = symbols;
        document_["symbols_bytes"] = symbols_bytes;
        document_["code_bytes"] = code_bytes;
        document_["jit_bytes"] = jit_bytes;
        document_["promoted_bytes"] = promoted_bytes;
        document_["promoted_objects"] = promoted_objects;
        document_["slab_refills"] = slab_refills;
        document_["slab_refills_fails"] = slab_refills_fails;
        document_["data_objects"] = data_objects;
        document_["capi_handles"] = capi_handles;
        document_["inflated_headers"] = inflated_headers;
        document_["suspend.ms"] = suspend_ms;
        document_["allocated_bytes"] = allocated_bytes;
        document_["malloc"] = malloc;
        document_["calloc"] = calloc;
        document_["realloc"] = realloc;
        document_["freed"] = freed;
      }

      virtual void start_reporting(STATE) {
        if(state->configuration()->diagnostics_memory_enabled) {
          Diagnostic::start_reporting(state);
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->configuration()->diagnostics_memory_enabled) {
          Diagnostic::stop_reporting(state);
        }
      }
    };
  }
}

#endif
