#include "class/iseq.hpp"
#include "vm.hpp"
#include "state.hpp"
#include "memory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestInstructionSequence : public CxxTest::TestSuite {
  public:

  VM *state;
  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

};
