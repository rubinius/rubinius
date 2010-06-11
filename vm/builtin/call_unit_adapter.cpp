#include "builtin/call_unit_adapter.hpp"
#include "builtin/system.hpp"
#include "builtin/class.hpp"
#include "builtin/location.hpp"

#include "dispatch.hpp"
#include "call_frame.hpp"
#include "arguments.hpp"

#include "object_utils.hpp"

namespace rubinius {
  CallUnitAdapter* CallUnitAdapter::create(STATE, Object* self, CallUnit* unit) {
    CallUnitAdapter* pe = state->new_object<CallUnitAdapter>(as<Class>(self));
    pe->unit(state, unit);

    pe->execute = adapter_executor;
    return pe;
  }

  Object* CallUnitAdapter::adapter_executor(STATE, CallFrame* call_frame, Dispatch& msg,
                                       Arguments& args)
  {
    CallUnitAdapter* adapter = as<CallUnitAdapter>(msg.method);
    CallUnit* unit = adapter->unit_;

    return unit->execute(state, call_frame, unit, msg, args);
  }
}
