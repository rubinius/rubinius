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

    // JIT/Interpreter
    config::Bool    dynamic_interpreter_enabled;
    config::Bool    jit_enabled;
    config::Integer jit_dump_code;
    config::Integer jit_call_til_compile;
    config::Integer jit_max_method_size;

    // Debug
    config::Bool    print_config;
    config::Bool    jit_show_compiling;
    config::Bool    gil_debug;

    // defaults
    static const int default_gc_bytes = 1048576 * 3;
    static const int default_gc_large_object = 2700;
    static const int default_gc_lifetime = 6;
    static const int default_jit_dump_code = 0;
    static const int default_jit_call_til_compile = 200;
    static const int default_jit_max_method_size = 2048;

    Configuration()
      : gc_bytes(this,        "rbx.gc.bytes", default_gc_bytes)
      , gc_large_object(this, "rbx.gc.large_object", default_gc_large_object)
      , gc_lifetime(this,     "rbx.gc.lifetime", default_gc_lifetime)
      , dynamic_interpreter_enabled(this, "interpreter.dynamic")
      , jit_enabled(this,     "jit.enabled")
      , jit_dump_code(this,   "jit.dump_code", default_jit_dump_code)
      , jit_call_til_compile(this, "jit.call_til_compile",
                             default_jit_call_til_compile)
      , jit_max_method_size(this, "jit.max_method_size",
                            default_jit_max_method_size)
      , print_config(this,    "config.print")
      , jit_show_compiling(this, "jit.show")
      , gil_debug(this,       "vm.gil.debug")
    {}
  };
}

#endif
