#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestSymbol : public CxxTest::TestSuite {
  public:

#undef state
  VM *state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_lookup() {
    OBJECT sym = state->globals.symbols->lookup(state, (unsigned char*)"blah", 4);
    TS_ASSERT(sym->symbol_p());

    OBJECT sym2 = state->globals.symbols->lookup(state, (unsigned char*)"blah", 4);
    TS_ASSERT_EQUALS(sym, sym2);
  }
};
