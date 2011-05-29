#ifndef RBX_CONFIGURATION_HPP
#define RBX_CONFIGURATION_HPP

#include "util/configuration.hpp"
#include "vm/config.h"

namespace rubinius {
  class Configuration : public config::Configuration {
  public:
    // GC
    config::Integer gc_bytes;
    config::Integer gc_large_object;
    config::Integer gc_lifetime;
    config::Bool    gc_autotune;
    config::Bool    gc_show;
    config::Bool    gc_immix_debug;
    config::Bool    gc_honor_start;
    config::Bool    gc_autopack;

    // Language
#ifdef RBX_ENABLED_18
    config::Bool    version_18;
#define LANGUAGE_18_ENABLED(state)  ((state)->shared.config.version_18)
#else
#define LANGUAGE_18_ENABLED(state)  (false)
#endif

#ifdef RBX_ENABLED_19
    config::Bool    version_19;
#define LANGUAGE_19_ENABLED(state)  ((state)->shared.config.version_19)
#else
#define LANGUAGE_19_ENABLED(state)  (false)
#endif

#ifdef RBX_ENABLED_20
    config::Bool    version_20;
#define LANGUAGE_20_ENABLED(state)  ((state)->shared.config.version_20)
#else
#define LANGUAGE_20_ENABLED(state)  (false)
#endif

    // JIT/Interpreter
    config::Bool    dynamic_interpreter_enabled;
    config::Integer jit_dump_code;
    config::Integer jit_call_til_compile;
    config::Integer jit_max_method_size;
    config::Bool    jit_show_compiling;
    config::Bool    jit_profile;
    config::Bool    jit_inline_generic;
    config::Bool    jit_inline_debug;
    config::Bool    jit_inline_blocks;
    config::String  jit_log;
    config::Bool    jit_disabled;
    config::Bool    jit_debug;
    config::Bool    jit_sync;
    config::Bool    jit_show_uncommon;
    config::Bool    jit_show_remove;
    config::Bool    jit_check_debugging;

    // Tools
    config::String  tool_to_load;

    // CAPI
    config::Bool    capi_global_flush;

    // Query Agent
    config::Integer qa_port;
    config::Bool    qa_verbose;
    config::String  qa_tmpdir;

    // Debug
    config::Bool    gil_debug;
    config::Integer print_config;
    config::Bool    ic_stats;
    config::Bool    profile;
    config::Integer profiler_threshold;
    config::String  report_path;
    config::Bool    thread_debug;

    // defaults
    static const int default_gc_bytes = 1048576 * 3;
    static const int default_gc_large_object = 2700;
    static const int default_gc_lifetime = 3;
    static const int default_jit_dump_code = 0;
    static const int default_jit_call_til_compile = 4000;
    static const int default_jit_max_method_size = 2048;
    static const bool default_jit_on = true;
    static const bool default_gc_autotune = true;

    Configuration()
      : gc_bytes(this,        "gc.bytes", default_gc_bytes)
      , gc_large_object(this, "gc.large_object", default_gc_large_object)
      , gc_lifetime(this,     "gc.lifetime", default_gc_lifetime)
      , gc_autotune(this,     "gc.autotune", default_gc_autotune)
      , gc_show(this,         "gc.show")
      , gc_immix_debug(this,  "gc.immix.debug")
      , gc_honor_start(this,  "gc.honor_start", false)
      , gc_autopack(this,     "gc.autopack", true)

#ifdef RBX_ENABLED_18
      , version_18(this, "18", RBX_DEFAULT_18)
#endif

#ifdef RBX_ENABLED_19
      , version_19(this, "19", RBX_DEFAULT_19)
#endif

#ifdef RBX_ENABLED_20
      , version_20(this, "20", RBX_DEFAULT_20)
#endif

      , dynamic_interpreter_enabled(this, "interpreter.dynamic")
      , jit_dump_code(this,   "jit.dump_code", default_jit_dump_code)
      , jit_call_til_compile(this, "jit.call_til_compile",
                             default_jit_call_til_compile)
      , jit_max_method_size(this, "jit.max_method_size",
                            default_jit_max_method_size)
      , jit_show_compiling(this, "jit.show")
      , jit_profile(this,     "jit.profile")
      , jit_inline_generic(this, "jit.inline.generic", true)
      , jit_inline_debug(this, "jit.inline.debug", false)
      , jit_inline_blocks(this, "jit.inline.blocks", true)
      , jit_log(this,        "jit.log")
      , jit_disabled(this,   "int")
      , jit_debug(this,      "jit.debug", false)
      , jit_sync(this,       "jit.sync", false)
      , jit_show_uncommon(this, "jit.uncommon.print", false)
      , jit_show_remove(this, "jit.removal.print", false)
      , jit_check_debugging(this, "jit.check_debugging", false)

      , tool_to_load(this, "tool")

      , capi_global_flush(this, "capi.global_flush", false)

      , qa_port(this,         "agent.start")
      , qa_verbose(this,      "agent.verbose")
      , qa_tmpdir(this,       "agent.tmpdir")
      , gil_debug(this,       "vm.gil.debug")
      , print_config(this,    "config.print")
      , ic_stats(this,        "ic.stats")
      , profile(this,         "profile")
      , profiler_threshold(this,  "profiler.threshold", 1000000)
      , report_path(this,     "vm.crash_report_path")
      , thread_debug(this,    "thread.debug")
    {
      gc_bytes.set_description(
          "The number of bytes the young generation of the GC should use");

      gc_large_object.set_description(
          "The size (in bytes) of the large object threshold");

      gc_lifetime.set_description(
          "How many young GC cycles an object lives before promoption");

      gc_autotune.set_description(
          "Set whether or not the GC should adjust itself for performance");

      gc_honor_start.set_description(
          "Control whether or not GC.start is honored when called");

      gc_autopack.set_description(
          "Set whether or not objects should be backed tightly in memory");

#ifdef RBX_ENABLED_18
      version_18.set_description(
          "Set the supported language version to 1.8");
#endif

#ifdef RBX_ENABLED_19
      version_19.set_description(
          "Set the supported language version to 1.9");
#endif

#ifdef RBX_ENABLED_20
      version_20.set_description(
          "Set the supported language version to 2.0");
#endif

      jit_dump_code.set_description(
          "1 == show simple IR, 2 == show optimized IR, 4 == show machine code");

      jit_call_til_compile.set_description(
          "How many times a method is called before the JIT is run on it");

      jit_max_method_size.set_description(
          "The max size of a method that will be JIT");

      jit_show_compiling.set_description(
          "Print out a status message when the JIT is operating");

      jit_profile.set_description(
          "The JIT will emit code to be sure JITd methods can be profile");

      jit_inline_generic.set_description(
          "Have the JIT inline generic methods");

      jit_inline_debug.set_description(
          "Have the JIT print out information about inlining");

      jit_inline_blocks.set_description(
          "Have the JIT try and inline methods and their literal blocks");

      jit_log.set_description(
          "Send JIT debugging output to this file rather than stdout");

      jit_disabled.set_description(
          "Force the JIT to never turn on");

      jit_sync.set_description(
          "Wait for the JIT to finish compiling each method");

      jit_show_uncommon.set_description(
          "Print out information on when methods are deoptimized due to uncommon traps");

      jit_show_remove.set_description(
          "Print out whenever the JIT is removing unused code");

      jit_check_debugging.set_description(
          "Allow JIT'd methods to deoptimize if there is a debugging request");

      capi_global_flush.set_description(
          "Flush all CAPI handles at CAPI call boundaries");

      print_config.set_description(
          "blank or 1 == names and values, 2 == description as well");

      gil_debug.set_description(
          "Print out debugging when the GIL is locked/unlocked");

      gc_show.set_description(
          "Print out whenever the GC runs");

      gc_immix_debug.set_description(
          "Print out collection stats when the Immix collector finishes");

      ic_stats.set_description(
          "Print out stats about the InlineCaches before exiting");

      qa_port.set_description(
          "Start the QueryAgent on a TCP port. Default port is a random port");

      qa_verbose.set_description(
          "Whether or not the query agent should print out status to stderr");

      qa_tmpdir.set_description(
          "Where to store files used to discover running query agents");

      profile.set_description(
          "Configure the system to profile ruby code");

      profiler_threshold.set_description(
          "The mininum number of nanoseconds a profiler node must have to be reported");

      report_path.set_description(
          "Set a custom path to write crash reports");

      thread_debug.set_description(
          "Print threading notices when they occur");
    }

    void finalize() { }
  };
}

#endif
