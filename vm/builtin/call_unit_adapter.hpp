#include "builtin/executable.hpp"
#include "builtin/call_unit.hpp"

namespace rubinius {
  struct CallFrame;
  class Dispatch;
  class Arguments;
  class BlockEnvironment;

  class CallUnitAdapter: public Executable {
  public:
    const static object_type type = CallUnitAdapterType;

  private:
    CallUnit* unit_; // slot

  public:
    attr_accessor(unit, CallUnit);

    // Rubinius.primitive :callunitadapter_create
    static CallUnitAdapter* create(STATE, Object* self, CallUnit* unit);

    static Object* adapter_executor(STATE, CallFrame* call_frame,
                           Executable* exec, Module* mod, Arguments& args);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };
  };
}
