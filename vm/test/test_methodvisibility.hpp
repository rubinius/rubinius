#include "builtin/methodvisibility.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestMethodVisibility : public CxxTest::TestSuite {
  public:

  VM *state;
  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_methodvisibility_fields() {
    TS_ASSERT_EQUALS(2U, MethodVisibility::fields);
  }
};
