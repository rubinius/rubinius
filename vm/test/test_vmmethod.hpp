#include "vm/test/test.hpp"

#include "vmmethod.hpp"

class TestVMMethod : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_create() {
    CompiledCode* cm = CompiledCode::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("blah"));
    cm->literals(state, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 1);
    iseq->opcodes()->put(state, 0, Fixnum::from(0));

    cm->iseq(state, iseq);

    VMMethod* vmm = new VMMethod(state, cm);

    TS_ASSERT_EQUALS(vmm->total, 1U);
    TS_ASSERT_EQUALS(vmm->opcodes[0], 0U);
  }

  void test_specialize_transforms_ivars_to_slots() {
    CompiledCode* cm = CompiledCode::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("@blah"));
    cm->literals(state, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 3);
    iseq->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_push_ivar));
    iseq->opcodes()->put(state, 1, Fixnum::from(0));
    iseq->opcodes()->put(state, 2, Fixnum::from(InstructionSequence::insn_push_nil));

    cm->iseq(state, iseq);

    VMMethod* vmm = new VMMethod(state, cm);

    Object::Info ti(ObjectType);
    ti.slots[state->symbol("@blah")->index()] = 5;
    ti.slot_locations.resize(6);
    ti.slot_locations[5] = 33;
    vmm->specialize(state, cm, &ti);

    TS_ASSERT_EQUALS(vmm->total, 3U);
    TS_ASSERT_EQUALS(vmm->opcodes[0], static_cast<unsigned int>(InstructionSequence::insn_push_my_offset));
    TS_ASSERT_EQUALS(vmm->opcodes[1], 33U);
    TS_ASSERT_EQUALS(vmm->opcodes[2], static_cast<unsigned int>(InstructionSequence::insn_push_nil));
  }

  void test_validate_ip() {
    CompiledCode* cm = CompiledCode::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("@blah"));
    cm->literals(state, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 3);
    iseq->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_push_ivar));
    iseq->opcodes()->put(state, 1, Fixnum::from(0));
    iseq->opcodes()->put(state, 2, Fixnum::from(InstructionSequence::insn_push_nil));

    cm->iseq(state, iseq);

    VMMethod* vmm = new VMMethod(state, cm);
    TS_ASSERT_EQUALS(vmm->validate_ip(state, 0), true);
    TS_ASSERT_EQUALS(vmm->validate_ip(state, 1), false);
    TS_ASSERT_EQUALS(vmm->validate_ip(state, 2), true);
  }
};
