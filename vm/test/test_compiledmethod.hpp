#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestCompiledMethod : public CxxTest::TestSuite {
  public:

  VM* state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_startup_tramp() {
    CompiledMethod* cm = CompiledMethod::generate_tramp(state);
    VMMethod* vmm = cm->vmmethod(state);

    TS_ASSERT_EQUALS(vmm->opcodes[0], static_cast<unsigned int>(InstructionSequence::insn_halt));
    TS_ASSERT(!cm->scope->nil_p());
    TS_ASSERT_EQUALS(cm->scope->module, G(object));
  }

};
