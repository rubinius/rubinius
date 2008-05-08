#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestCompiledMethod : public CxxTest::TestSuite {
  public:

#undef state
  VM* state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

};
