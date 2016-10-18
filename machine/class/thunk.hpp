#ifndef RBX_BUILTIN_THUNK_HPP
#define RBX_BUILTIN_THUNK_HPP

#include "class/executable.hpp"

namespace rubinius {
  struct CallFrame;
  class Dispatch;
  class Arguments;
  class BlockEnvironment;

  class Thunk : public Executable {
  public:
    const static object_type type = ThunkType;

    attr_accessor(value, Object);

    static void initialize(STATE, Thunk* obj) {
      Executable::initialize(state, obj);

      obj->value(nil<Object>());
    }

    // Rubinius.primitive :thunk_create
    static Thunk* create(STATE, Object* self, Object* value);

    static Object* thunk_executor(STATE,
                           Executable* exec, Module* mod, Arguments& args);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };
  };
}

#endif
