#include "vm/test/test.hpp"

#include "machine_code.hpp"

class TestMachineCode : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_create() {
    CompiledCode* code = CompiledCode::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("blah"));
    code->literals(state, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 1);
    iseq->opcodes()->put(state, 0, Fixnum::from(0));

    code->iseq(state, iseq);

    MachineCode* mcode = new MachineCode(state, code);

    TS_ASSERT_EQUALS(mcode->total, 1U);
    TS_ASSERT_EQUALS(mcode->opcodes[0], 0U);
  }

  void test_specialize_transforms_ivars_to_slots() {
    CompiledCode* code = CompiledCode::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("@blah"));
    code->literals(state, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 3);
    iseq->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_push_ivar));
    iseq->opcodes()->put(state, 1, Fixnum::from(0));
    iseq->opcodes()->put(state, 2, Fixnum::from(InstructionSequence::insn_push_nil));

    code->iseq(state, iseq);

    MachineCode* mcode = new MachineCode(state, code);

    Object::Info ti(ObjectType);
    ti.slots[state->symbol("@blah")->index()] = 5;
    ti.slot_locations.resize(6);
    ti.slot_locations[5] = 33;
    mcode->specialize(state, code, &ti);

    TS_ASSERT_EQUALS(mcode->total, 3U);
    TS_ASSERT_EQUALS(mcode->opcodes[0], static_cast<unsigned int>(InstructionSequence::insn_push_my_offset));
    TS_ASSERT_EQUALS(mcode->opcodes[1], 33U);
    TS_ASSERT_EQUALS(mcode->opcodes[2], static_cast<unsigned int>(InstructionSequence::insn_push_nil));
  }

  void test_validate_ip() {
    CompiledCode* code = CompiledCode::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("@blah"));
    code->literals(state, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 3);
    iseq->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_push_ivar));
    iseq->opcodes()->put(state, 1, Fixnum::from(0));
    iseq->opcodes()->put(state, 2, Fixnum::from(InstructionSequence::insn_push_nil));

    code->iseq(state, iseq);

    MachineCode* mcode = new MachineCode(state, code);
    TS_ASSERT_EQUALS(mcode->validate_ip(state, 0), true);
    TS_ASSERT_EQUALS(mcode->validate_ip(state, 1), false);
    TS_ASSERT_EQUALS(mcode->validate_ip(state, 2), true);
  }
};
