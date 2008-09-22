#include "builtin/nativemethodcontext.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestNativeMethodContext : public CxxTest::TestSuite {
  public:

  VM *state;
  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_nativemethodcontext_fields() {
    TS_ASSERT_EQUALS(0U, NativeMethodContext::fields);
  }
};
