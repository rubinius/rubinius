#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestVM : public CxxTest::TestSuite {
  public:

#undef state
  VM *state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_symbol() {
    Symbol *sym1 = state->symbol("blah");
    Symbol *sym2 = state->symbol("blah");

    TS_ASSERT_EQUALS(sym1, sym2);
  }
};
