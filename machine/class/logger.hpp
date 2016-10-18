#ifndef RBX_BUILTIN_LOGGER_HPP
#define RBX_BUILTIN_LOGGER_HPP

#include "object_utils.hpp"

#include "class/object.hpp"
#include "class/string.hpp"

namespace rubinius {
  class String;

  class Logger : public Object {
  public:
    const static object_type type = LoggerType;

    attr_accessor(name, String);
    attr_accessor(format, String);

    static void bootstrap(STATE);
    static void initialize(STATE, Logger* obj) {
      obj->name(nil<String>());
      obj->format(nil<String>());
    }

    static Logger* create(STATE);

    // Rubinius.primitive+ :logger_allocate
    static Logger* allocate(STATE, Object* self);

    // Rubinius.primitive :logger_write
    Object* write(STATE, String* message);

    // Rubinius.primitive :logger_fatal
    Object* fatal(STATE, String* message);

    // Rubinius.primitive :logger_error
    Object* error(STATE, String* message);

    // Rubinius.primitive :logger_warn
    Object* warn(STATE, String* message);

    // Rubinius.primitive :logger_info
    Object* info(STATE, String* message);

    // Rubinius.primitive :logger_debug
    Object* debug(STATE, String* message);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
