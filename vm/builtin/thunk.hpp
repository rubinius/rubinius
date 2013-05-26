#ifndef RBX_BUILTIN_THUNK_HPP
#define RBX_BUILTIN_THUNK_HPP

#include "builtin/executable.hpp"

namespace rubinius {
  struct CallFrame;
  class Dispatch;
  class Arguments;
  class BlockEnvironment;

  class Thunk : public Executable {
  public:
    const static object_type type = ThunkType;

  private:
    Object* value_; // slot

  public:
    attr_accessor(value, Object);

    // Rubinius.primitive :thunk_create
    static Thunk* create(STATE, Object* self, Object* value);

    static Object* thunk_executor(STATE, CallFrame* call_frame,
                           Executable* exec, Module* mod, Arguments& args);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };
  };
}

#endif
