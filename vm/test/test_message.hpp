#include "vm/test/test.hpp"

#include "builtin/tuple.hpp"
#include "builtin/list.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/array.hpp"
#include "builtin/iseq.hpp"
#include "builtin/compiledcode.hpp"
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

  CompiledCode* create_compiled_code() {
    CompiledCode* code = CompiledCode::create(state);
    code->iseq(state, InstructionSequence::create(state, 1));
    code->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    code->stack_size(state, Fixnum::from(10));
    code->total_args(state, Fixnum::from(0));
    code->required_args(state, code->total_args());
    code->formalize(state);

    return code;
  }

};
