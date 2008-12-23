
#include "vmmethod.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestVMMethod : public CxxTest::TestSuite {
public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    CompiledMethod* cm = CompiledMethod::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("blah"));
    cm->literals(state, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 1);
    iseq->opcodes()->put(state, 0, Fixnum::from(0));

    cm->iseq(state, iseq);

    VMMethod vmm(state, cm);

    TS_ASSERT_EQUALS(vmm.total, 1U);
    TS_ASSERT_EQUALS(vmm.opcodes[0], 0U);
  }

  void test_specialize_transforms_ivars_to_slots() {
    CompiledMethod* cm = CompiledMethod::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("@blah"));
    cm->literals(state, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 3);
    iseq->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_push_ivar));
    iseq->opcodes()->put(state, 1, Fixnum::from(0));
    iseq->opcodes()->put(state, 2, Fixnum::from(InstructionSequence::insn_push_nil));

    cm->iseq(state, iseq);

    VMMethod vmm(state, cm);

    TypeInfo ti(ObjectType);
    ti.slots[state->symbol("@blah")->index()] = 5;
    vmm.specialize(state, &ti);

    TS_ASSERT_EQUALS(vmm.total, 3U);
    TS_ASSERT_EQUALS(vmm.opcodes[0], static_cast<unsigned int>(InstructionSequence::insn_push_my_field));
    TS_ASSERT_EQUALS(vmm.opcodes[1], 5U);
    TS_ASSERT_EQUALS(vmm.opcodes[2], static_cast<unsigned int>(InstructionSequence::insn_push_nil));
  }

  void test_set_breakpoint_flags() {
    CompiledMethod* cm = CompiledMethod::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("@blah"));
    cm->literals(state, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 3);
    iseq->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_push_ivar));
    iseq->opcodes()->put(state, 1, Fixnum::from(0));
    iseq->opcodes()->put(state, 2, Fixnum::from(InstructionSequence::insn_push_nil));

    cm->iseq(state, iseq);

    VMMethod vmm(state, cm);

    vmm.set_breakpoint_flags(state, 0, 1 << 24);
    TS_ASSERT_EQUALS(vmm.opcodes[0], (1 << 24) | static_cast<unsigned int>(InstructionSequence::insn_push_ivar));

    vmm.set_breakpoint_flags(state, 0, 7 << 24);
    TS_ASSERT_EQUALS(vmm.opcodes[0], (7 << 24) | static_cast<unsigned int>(InstructionSequence::insn_push_ivar));

    vmm.set_breakpoint_flags(state, 0, 0);
    TS_ASSERT_EQUALS(vmm.opcodes[0], static_cast<unsigned int>(InstructionSequence::insn_push_ivar));

    TS_ASSERT_THROWS(vmm.set_breakpoint_flags(state, 1, 1), const RubyException &e);
  }

  void test_get_breakpoint_flags() {
    CompiledMethod* cm = CompiledMethod::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("@blah"));
    cm->literals(state, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 3);
    iseq->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_push_ivar));
    iseq->opcodes()->put(state, 1, Fixnum::from(0));
    iseq->opcodes()->put(state, 2, Fixnum::from(4 << 24 | InstructionSequence::insn_push_nil));

    cm->iseq(state, iseq);

    VMMethod vmm(state, cm);

    TS_ASSERT_EQUALS(vmm.get_breakpoint_flags(state, 0), 0);
    TS_ASSERT_EQUALS(vmm.get_breakpoint_flags(state, 2), 4 << 24);

    TS_ASSERT_THROWS(vmm.get_breakpoint_flags(state, 1), const RubyException &e);
  }
};
