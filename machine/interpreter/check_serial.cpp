#include "instructions/check_serial.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t check_serial(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);
      intptr_t serial = argument(1);

      instructions::check_serial(state, call_frame, literal, serial);

      call_frame->next_ip(instructions::data_check_serial.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
