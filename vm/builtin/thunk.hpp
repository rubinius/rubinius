#include "builtin/executable.hpp"

namespace rubinius {
  class CallFrame;
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

    // Ruby.primitive :thunk_create
    static Thunk* create(STATE, Object* self, Object* value);

    static Object* thunk_executor(STATE, CallFrame* call_frame,
                           Dispatch& msg, Arguments& args);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };
  };
}
