#include "call_frame.hpp"
#include "interpreter.hpp"
#include "machine_code.hpp"

#include "interpreter/addresses.hpp"

namespace rubinius {
  void Interpreter::prepare(STATE, MachineCode* machine_code) {
  }

  intptr_t Interpreter::execute(STATE, MachineCode* const machine_code) {
    return 0;
  }
}
