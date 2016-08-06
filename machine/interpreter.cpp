#include "interpreter.hpp"
#include "machine_code.hpp"

#include "interpreter/addresses.hpp"

#include "builtin/location.hpp"

namespace rubinius {
  void Interpreter::prepare(STATE, MachineCode* machine_code) {
  }

  intptr_t Interpreter::execute(STATE, MachineCode* const machine_code) {
    InterpreterState is;
    UnwindInfoSet unwinds;
    Exception* exception = 0;
    intptr_t* opcodes = (intptr_t*)machine_code->opcodes;
    CallFrame* call_frame = state->vm()->call_frame();

    state->vm()->call_frame()->is = &is;
    state->vm()->call_frame()->unwinds = &unwinds;

    try {
      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    } catch(TypeError& e) {
      exception = Exception::make_type_error(state, e.type, e.object, e.reason);
      exception->locations(state, Location::from_call_stack(state));

      call_frame->scope->flush_to_heap(state);
    } catch(const RubyException& exc) {
      if(exc.exception->locations()->nil_p()) {
        exc.exception->locations(state, Location::from_call_stack(state));
      }
      exception = exc.exception;
    }

    state->raise_exception(exception);
    return NULL;
  }
}
