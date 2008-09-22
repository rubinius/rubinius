#include "builtin/staticscope.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestStaticScope : public CxxTest::TestSuite {
  public:

  VM *state;
  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_staticscope_fields() {
    TS_ASSERT_EQUALS(2U, StaticScope::fields);
  }
};
