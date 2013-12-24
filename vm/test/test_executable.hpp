#include "builtin/executable.hpp"
#include "vm.hpp"
#include "object_memory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestExecutable : public CxxTest::TestSuite {
  public:

  VM *state;
  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

};
