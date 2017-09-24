#include "memory.hpp"

#include "class/class.hpp"
#include "class/iseq.hpp"
#include "class/tuple.hpp"

namespace rubinius {
  void InstructionSequence::bootstrap(STATE) {
    GO(iseq).set(state->memory()->new_class<Class, InstructionSequence>(
          state, G(rubinius), "InstructionSequence"));
  }

  InstructionSequence* InstructionSequence::create(STATE, size_t instructions) {
    InstructionSequence* is = InstructionSequence::allocate(state, G(iseq));

    is->opcodes(state, Tuple::create(state, instructions));

    return is;
  }

  InstructionSequence* InstructionSequence::allocate(STATE, Object* self) {
    return state->memory()->new_object<InstructionSequence>(state, as<Class>(self));
  }

  void InstructionSequence::post_marshal(STATE) {

  }

  void InstructionSequence::Info::show(STATE, Object* self, int level) {
    InstructionSequence* iseq = as<InstructionSequence>(self);

    class_header(state, self);
    indent_attribute(++level, "opcodes"); iseq->opcodes()->show(state, level);
    close_body(level);
  }
}
