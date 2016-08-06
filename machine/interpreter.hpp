#ifndef RBX_INTERPRETER_HPP
#define RBX_INTERPRETER_HPP

#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"

#include "interpreter/prototypes.hpp"

namespace rubinius {
  struct CallFrame;
  class MachineCode;

  class Interpreter {
    static const intptr_t instruction_addresses[];

  public:

    typedef intptr_t (*Instruction)(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

    static void prepare(STATE, MachineCode* machine_code);
    static intptr_t execute(STATE, MachineCode* const machine_code);
  };
}

#endif
