#ifndef RBX_DIAGNOSTICS_MACHINE_HPP
#define RBX_DIAGNOSTICS_MACHINE_HPP

#include "diagnostics/diagnostic.hpp"

namespace rubinius {
  namespace diagnostics {
    class BootMetrics : public Diagnostic {
    public:
      metric fields_us;
      metric main_thread_us;
      metric memory_us;
      metric ontology_us;
      metric platform_us;

      BootMetrics()
        : Diagnostic()
        , fields_us(0)
        , main_thread_us(0)
        , memory_us(0)
        , ontology_us(0)
        , platform_us(0)
      {
        set_type("BootMetrics");

        document_.AddMember("fields.us", fields_us, document_.GetAllocator());
        document_.AddMember("main_thread.us", main_thread_us, document_.GetAllocator());
        document_.AddMember("memory.us", memory_us, document_.GetAllocator());
        document_.AddMember("ontology.us", ontology_us, document_.GetAllocator());
        document_.AddMember("platform.us", platform_us, document_.GetAllocator());
      }

      virtual void update() {
        document_["fields.us"] = fields_us;
        document_["main_thread.us"] = main_thread_us;
        document_["memory.us"] = memory_us;
        document_["ontology.us"] = ontology_us;
        document_["platform.us"] = platform_us;
      }
    };

    class MachineMetrics : public Diagnostic {
    public:
      metric checkpoints;
      metric stops;
      metric call_site_count;
      metric inline_cache_count;
      metric inline_cache_disabled;
      metric cache_resets;
      metric methods_invoked;
      metric blocks_invoked;
      metric backtraces;
      metric backtrace_us;
      metric bytecode_load_us;
      metric bytecode_verifier_us;
      metric bytecode_internalizer_us;
      metric samples;
      metric sample_ns;
      metric profiles;
      metric profile_ns;
      metric unwind_site_count;
      metric unwind_site_full;
      metric unwind_site_polymorphic;
      metric read_bytes;
      metric write_bytes;
      metric signals_received;
      metric signals_processed;
      metric threads_created;
      metric threads_destroyed;
      metric fibers_created;
      metric fibers_destroyed;

      MachineMetrics()
        : Diagnostic()
        , checkpoints(0)
        , stops(0)
        , call_site_count(0)
        , inline_cache_count(0)
        , inline_cache_disabled(0)
        , cache_resets(0)
        , methods_invoked(0)
        , blocks_invoked(0)
        , backtraces(0)
        , backtrace_us(0)
        , bytecode_load_us(0)
        , bytecode_verifier_us(0)
        , bytecode_internalizer_us(0)
        , samples(0)
        , sample_ns(0)
        , profiles(0)
        , profile_ns(0)
        , unwind_site_count(0)
        , unwind_site_full(0)
        , unwind_site_polymorphic(0)
        , read_bytes(0)
        , write_bytes(0)
        , signals_received(0)
        , signals_processed(0)
        , threads_created(0)
        , threads_destroyed(0)
        , fibers_created(0)
        , fibers_destroyed(0)
      {
        set_type("MachineMetrics");

        document_.AddMember("checkpoints", checkpoints, document_.GetAllocator());
        document_.AddMember("stops", stops, document_.GetAllocator());
        document_.AddMember("call_site_count", call_site_count, document_.GetAllocator());
        document_.AddMember("inline_cache_count", inline_cache_count, document_.GetAllocator());
        document_.AddMember("inline_cache_disabled", inline_cache_disabled, document_.GetAllocator());
        document_.AddMember("cache_resets", cache_resets, document_.GetAllocator());
        document_.AddMember("methods_invoked", methods_invoked, document_.GetAllocator());
        document_.AddMember("blocks_invoked", blocks_invoked, document_.GetAllocator());
        document_.AddMember("backtraces", backtraces, document_.GetAllocator());
        document_.AddMember("backtrace_us", backtrace_us, document_.GetAllocator());
        document_.AddMember("bytecode_load_us", bytecode_load_us, document_.GetAllocator());
        document_.AddMember("bytecode_verifier_us", bytecode_verifier_us, document_.GetAllocator());
        document_.AddMember("bytecode_internalizer_us", bytecode_internalizer_us, document_.GetAllocator());
        document_.AddMember("samples", samples, document_.GetAllocator());
        document_.AddMember("sample_ns", sample_ns, document_.GetAllocator());
        document_.AddMember("profiles", profiles, document_.GetAllocator());
        document_.AddMember("profile_ns", profile_ns, document_.GetAllocator());
        document_.AddMember("unwind_site_count", unwind_site_count, document_.GetAllocator());
        document_.AddMember("unwind_site_full", unwind_site_full, document_.GetAllocator());
        document_.AddMember("unwind_site_polymorphic", unwind_site_polymorphic, document_.GetAllocator());
        document_.AddMember("read_bytes", read_bytes, document_.GetAllocator());
        document_.AddMember("write_bytes", write_bytes, document_.GetAllocator());
        document_.AddMember("signals_received", signals_received, document_.GetAllocator());
        document_.AddMember("signals_processed", signals_processed, document_.GetAllocator());
        document_.AddMember("threads_created", threads_created, document_.GetAllocator());
        document_.AddMember("threads_destroyed", threads_destroyed, document_.GetAllocator());
        document_.AddMember("fibers_created", fibers_created, document_.GetAllocator());
        document_.AddMember("fibers_destroyed", fibers_destroyed, document_.GetAllocator());
      }

      virtual void update() {
        document_["checkpoints"] = checkpoints;
        document_["stops"] = stops;
        document_["call_site_count"] = call_site_count;
        document_["inline_cache_count"] = inline_cache_count;
        document_["inline_cache_disabled"] = inline_cache_disabled;
        document_["cache_resets"] = cache_resets;
        document_["methods_invoked"] = methods_invoked;
        document_["blocks_invoked"] = blocks_invoked;
        document_["backtraces"] = backtraces;
        document_["backtrace_us"] = backtrace_us;
        document_["bytecode_load_us"] = bytecode_load_us;
        document_["bytecode_verifier_us"] = bytecode_verifier_us;
        document_["bytecode_internalizer_us"] = bytecode_internalizer_us;
        document_["samples"] = samples;
        document_["sample_ns"] = sample_ns;
        document_["profiles"] = profiles;
        document_["profile_ns"] = profile_ns;
        document_["unwind_site_count"] = unwind_site_count;
        document_["unwind_site_full"] = unwind_site_full;
        document_["unwind_site_polymorphic"] = unwind_site_polymorphic;
        document_["read_bytes"] = read_bytes;
        document_["write_bytes"] = write_bytes;
        document_["signals_received"] = signals_received;
        document_["signals_processed"] = signals_processed;
        document_["threads_created"] = threads_created;
        document_["threads_destroyed"] = threads_destroyed;
        document_["fibers_created"] = fibers_created;
        document_["fibers_destroyed"] = fibers_destroyed;
      }

      virtual void start_reporting(STATE) {
        if(state->shared().config.diagnostics_machine_enabled) {
          Diagnostic::start_reporting(state);
        }
      }

      virtual void stop_reporting(STATE) {
        if(state->shared().config.diagnostics_machine_enabled) {
          Diagnostic::stop_reporting(state);
        }
      }
    };
  }
}

#endif
