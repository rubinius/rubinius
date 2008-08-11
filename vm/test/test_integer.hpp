#include "objects.hpp"
#include "builtin/list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestInteger : public CxxTest::TestSuite {
  public:

  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_from_native_int() {
    // TODO
  }

  void test_from_int() {
    // TODO
  }

  void test_from_unsigned_int() {
    // TODO
  }

  void test_from_unsigned_long() {
    // TODO
  }

  void test_from_long_long() {
    // TODO
  }

  void test_from_unsigned_long_long() {
    // TODO
  }
};