#include "vm/test/test.hpp"

#include "builtin/tuple.hpp"
#include "builtin/list.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/array.hpp"
#include "builtin/iseq.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/symbol.hpp"

#include "message.hpp"

class TestMessage : public CxxTest::TestSuite, public VMTest {
public:

  Object** args;
  Fixnum* three;
  Fixnum* four;

  void setUp() {
    create();

    three = Fixnum::from(3);
    four = Fixnum::from(4);

    args = new Object*[2];
    args[0] = three;
    args[1] = four;
  }

  void tearDown() {
    delete args;
    destroy();
  }

  CompiledMethod* create_cm() {
    CompiledMethod* cm = CompiledMethod::create(state);
    cm->iseq(state, InstructionSequence::create(state, 1));
    cm->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    cm->stack_size(state, Fixnum::from(10));
    cm->total_args(state, Fixnum::from(0));
    cm->required_args(state, cm->total_args());
    cm->formalize(state);

    return cm;
  }

};
