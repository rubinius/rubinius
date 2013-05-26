#include "builtin/call_unit_adapter.hpp"
#include "builtin/call_unit.hpp"
#include "builtin/class.hpp"
#include "object_utils.hpp"

namespace rubinius {
  CallUnitAdapter* CallUnitAdapter::create(STATE, Object* self, CallUnit* unit) {
    CallUnitAdapter* pe = state->new_object<CallUnitAdapter>(as<Class>(self));
    pe->unit(state, unit);
    pe->inliners_ = 0;
    pe->prim_index_ = -1;
    pe->custom_call_site_ = false;
    pe->execute = adapter_executor;
    return pe;
  }

  Object* CallUnitAdapter::adapter_executor(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
                                       Arguments& args)
  {
    CallUnitAdapter* adapter = as<CallUnitAdapter>(exec);
    CallUnit* unit = adapter->unit_;

    return unit->execute(state, call_frame, unit, exec, mod, args);
  }
}
