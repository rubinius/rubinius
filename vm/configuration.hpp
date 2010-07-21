#ifndef RBX_CONFIGURATION_HPP
#define RBX_CONFIGURATION_HPP

#include "util/configuration.hpp"

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

    // Query Agent
    config::Integer qa_port;
    config::Bool    qa_verbose;
    config::String  qa_tmpdir;

    // Debug
    config::Bool    gil_debug;
    config::Integer print_config;
    config::Bool    ic_stats;
    config::Bool    profile;

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

      , dynamic_interpreter_enabled(this, "interpreter.dynamic")
      , jit_dump_code(this,   "jit.dump_code", default_jit_dump_code)
      , jit_call_til_compile(this, "jit.call_til_compile",
                             default_jit_call_til_compile)
      , jit_max_method_size(this, "jit.max_method_size",
                            default_jit_max_method_size)
      , jit_show_compiling(this, "jit.show")
      , jit_profile(this,     "jit.profile")
      , jit_inline_generic(this, "jit.inline.generic", true)
      , jit_inline_debug(this, "jit.inline.debug")
      , jit_inline_blocks(this, "jit.inline.blocks")
      , jit_log(this,        "jit.log")
      , jit_disabled(this,   "int")
      , jit_debug(this,      "jit.debug", false)
      , jit_sync(this,       "jit.sync", false)
      , jit_show_uncommon(this, "jit.uncommon.print", false)
      , jit_show_remove(this, "jit.removal.print", false)
      , qa_port(this,         "agent.start")
      , qa_verbose(this,      "agent.verbose")
      , qa_tmpdir(this,       "agent.tmpdir")
      , gil_debug(this,       "vm.gil.debug")
      , print_config(this,    "config.print")
      , ic_stats(this,        "ic.stats")
      , profile(this,         "profile")
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
    }

    void finalize() {
      if(profile) {
        jit_disabled.value = true;
      }
    }
  };
}

#endif
