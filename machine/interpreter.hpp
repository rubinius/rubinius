#ifndef RBX_INTERPRETER_HPP
#define RBX_INTERPRETER_HPP

#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"
#include "instructions.hpp"

namespace rubinius {
  struct CallFrame;
  class CompiledCode;
  class MachineCode;

  class Interpreter {
    static const intptr_t interpreter_addresses[];

  public:

    static void prepare(STATE, CompiledCode* compiled_code, MachineCode* machine_code);
    static intptr_t execute(STATE, MachineCode* const machine_code);
  };
}

#endif
