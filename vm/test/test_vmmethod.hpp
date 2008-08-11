
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
    SET(cm, literals, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 1);
    iseq->opcodes->put(state, 0, Fixnum::from(0));

    SET(cm, iseq, iseq);
   
    VMMethod vmm(state, cm);

    TS_ASSERT_EQUALS(vmm.total, 1U);
    TS_ASSERT_EQUALS(vmm.opcodes[0], 0U);
  }

  void test_specialize_transforms_ivars_to_slots() {
    CompiledMethod* cm = CompiledMethod::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("@blah"));
    SET(cm, literals, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 3);
    iseq->opcodes->put(state, 0, Fixnum::from(InstructionSequence::insn_push_ivar));
    iseq->opcodes->put(state, 1, Fixnum::from(0));
    iseq->opcodes->put(state, 2, Fixnum::from(InstructionSequence::insn_push_nil));

    SET(cm, iseq, iseq);

    VMMethod vmm(state, cm);

    TypeInfo ti(ObjectType);
    ti.slots[state->symbol("@blah")->index()] = 5;
    vmm.specialize(&ti);

    TS_ASSERT_EQUALS(vmm.total, 3U);
    TS_ASSERT_EQUALS(vmm.opcodes[0], static_cast<unsigned int>(InstructionSequence::insn_push_my_field));
    TS_ASSERT_EQUALS(vmm.opcodes[1], 5U);
    TS_ASSERT_EQUALS(vmm.opcodes[2], static_cast<unsigned int>(InstructionSequence::insn_push_nil));
  }

};
