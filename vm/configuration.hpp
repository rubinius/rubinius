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
    config::Bool    gc_show;

    // JIT/Interpreter
    config::Bool    dynamic_interpreter_enabled;
    config::Bool    jit_enabled;
    config::Integer jit_dump_code;
    config::Integer jit_call_til_compile;
    config::Integer jit_max_method_size;
    config::Bool    jit_show_compiling;

    // Debug
    config::Bool    gil_debug;
    config::Integer print_config;

    // defaults
    static const int default_gc_bytes = 1048576 * 3;
    static const int default_gc_large_object = 2700;
    static const int default_gc_lifetime = 6;
    static const int default_jit_dump_code = 0;
    static const int default_jit_call_til_compile = 200;
    static const int default_jit_max_method_size = 2048;

    Configuration()
      : gc_bytes(this,        "gc.bytes", default_gc_bytes)
      , gc_large_object(this, "gc.large_object", default_gc_large_object)
      , gc_lifetime(this,     "gc.lifetime", default_gc_lifetime)
      , gc_show(this,         "gc.show")
      , dynamic_interpreter_enabled(this, "interpreter.dynamic")
      , jit_enabled(this,     "jit.enabled")
      , jit_dump_code(this,   "jit.dump_code", default_jit_dump_code)
      , jit_call_til_compile(this, "jit.call_til_compile",
                             default_jit_call_til_compile)
      , jit_max_method_size(this, "jit.max_method_size",
                            default_jit_max_method_size)
      , jit_show_compiling(this, "jit.show")
      , gil_debug(this,       "vm.gil.debug")
      , print_config(this,    "config.print")
    {
      gc_bytes.set_description(
          "The number of bytes the young generation of the GC should use");

      gc_large_object.set_description(
          "The size (in bytes) of the large object threshold");

      gc_lifetime.set_description(
          "How many young GC cycles an object lives before promoption");

      jit_enabled.set_description(
          "Whether or not to use the dynamic JIT");

      jit_dump_code.set_description(
          "1 == show simple IR, 2 == show optimized IR, 4 == show machine code");

      jit_call_til_compile.set_description(
          "How many times a method is called before the JIT is run on it");

      jit_max_method_size.set_description(
          "The max size of a method that will be JIT");

      print_config.set_description(
          "blank or 1 == names and values, 2 == description as well");

      jit_show_compiling.set_description(
          "Print out a status message when the JIT is operating");

      gil_debug.set_description(
          "Print out debugging when the GIL is locked/unlocked");

      gc_show.set_description(
          "Print out whenever the GC runs");
    }
  };
}

#endif
