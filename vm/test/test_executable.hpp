#include "builtin/executable.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

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

  void test_executable_fields() {
    TS_ASSERT_EQUALS(3U, Executable::fields);
  }
};
