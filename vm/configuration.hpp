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
    config::Bool    jit_enabled;
    config::Bool    dynamic_interpreter_enabled;
    config::Bool    jit_dump_code;

    // Debug
    config::Bool    print_config;

    // defaults
    static const int default_gc_bytes = 1048576 * 3;
    static const int default_gc_large_object = 2700;
    static const int default_gc_lifetime = 6;

    Configuration()
      : gc_bytes(this,        "rbx.gc.bytes", default_gc_bytes)
      , gc_large_object(this, "rbx.gc.large_object", default_gc_large_object)
      , gc_lifetime(this,     "rbx.gc.lifetime", default_gc_lifetime)
      , jit_enabled(this,     "rbx.jit.enabled")
      , dynamic_interpreter_enabled(this, "rbx.interpreter.dynamic")
      , jit_dump_code(this,   "rbx.jit.dump_code")
      , print_config(this,    "config.print")
    {}
  };
}

#endif
