#include "object_utils.hpp"
#include "memory.hpp"

#include "builtin/call_unit_adapter.hpp"
#include "builtin/call_unit.hpp"
#include "builtin/class.hpp"

namespace rubinius {
  CallUnitAdapter* CallUnitAdapter::create(STATE, Object* self, CallUnit* unit) {
    CallUnitAdapter* pe =
      state->memory()->new_object<CallUnitAdapter>(state, as<Class>(self));

    pe->unit(state, unit);

    return pe;
  }

  Object* CallUnitAdapter::adapter_executor(STATE, CallFrame* call_frame,
      Executable* exec, Module* mod, Arguments& args)
  {
    CallUnitAdapter* adapter = as<CallUnitAdapter>(exec);
    CallUnit* unit = adapter->unit_;

    return unit->execute(state, call_frame, unit, exec, mod, args);
  }
}
