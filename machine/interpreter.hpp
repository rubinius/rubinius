#ifndef RBX_INTERPRETER_HPP
#define RBX_INTERPRETER_HPP

#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"

#include "interpreter/instructions.hpp"
#include "interpreter/prototypes.hpp"

namespace rubinius {
  struct CallFrame;
  class CompiledCode;
  class MachineCode;

  class Interpreter {
    static const intptr_t instruction_addresses[];
    static const instructions::InstructionData instruction_data[];

  public:

    static const instructions::InstructionData& instruction_data_(int index) {
      return instruction_data[index];
    }

    static void prepare(STATE, CompiledCode* compiled_code, MachineCode* machine_code);
    static intptr_t execute(STATE, MachineCode* const machine_code);
  };
}

#endif
