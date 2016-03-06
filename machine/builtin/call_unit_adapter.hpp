#ifndef RBX_BUILTIN_CALL_UNIT_ADAPTER_HPP
#define RBX_BUILTIN_CALL_UNIT_ADAPTER_HPP

#include "object_utils.hpp"

#include "builtin/executable.hpp"

namespace rubinius {
  struct CallFrame;
  class Arguments;
  class CallUnit;

  class CallUnitAdapter: public Executable {
  public:
    const static object_type type = CallUnitAdapterType;

  private:
    CallUnit* unit_; // slot

  public:
    attr_accessor(unit, CallUnit);

    static void initialize(STATE, CallUnitAdapter* obj) {
      Executable::initialize(state, obj);

      obj->unit_ = nil<CallUnit>();
    }

    // Rubinius.primitive :callunitadapter_create
    static CallUnitAdapter* create(STATE, Object* self, CallUnit* unit);

    static Object* adapter_executor(STATE,
                           Executable* exec, Module* mod, Arguments& args);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };
  };
}

#endif
