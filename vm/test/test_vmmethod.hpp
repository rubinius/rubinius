#include "builtin.hpp"
#include "vmmethod.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestVMMethod : public CxxTest::TestSuite {
public:

#undef state
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
    iseq->opcodes->put(state, 0, Object::i2n(0));

    SET(cm, iseq, iseq);
   
    VMMethod vmm(state, cm);

    TS_ASSERT_EQUALS(vmm.total, 1);
    TS_ASSERT_EQUALS(vmm.opcodes[0], 0);
  }

  void test_specialize_transforms_ivars_to_slots() {
    CompiledMethod* cm = CompiledMethod::create(state);
    Tuple* tup = Tuple::from(state, 1, state->symbol("@blah"));
    SET(cm, literals, tup);

    InstructionSequence* iseq = InstructionSequence::create(state, 3);
    iseq->opcodes->put(state, 0, Object::i2n(InstructionSequence::insn_push_ivar));
    iseq->opcodes->put(state, 1, Object::i2n(0));
    iseq->opcodes->put(state, 2, Object::i2n(InstructionSequence::insn_push_nil));

    SET(cm, iseq, iseq);

    VMMethod vmm(state, cm);

    TypeInfo ti(ObjectType);
    ti.slots[state->symbol("@blah")->index()] = 5;
    vmm.specialize(&ti);

    TS_ASSERT_EQUALS(vmm.total, 3);
    TS_ASSERT_EQUALS(vmm.opcodes[0], (int)InstructionSequence::insn_push_my_field);
    TS_ASSERT_EQUALS(vmm.opcodes[1], 5);
    TS_ASSERT_EQUALS(vmm.opcodes[2], (int)InstructionSequence::insn_push_nil);
  }

};
