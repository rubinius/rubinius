#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestSymbol : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_index() {
    Symbol* sym = state->symbols.lookup(state, "blah");

    TS_ASSERT_EQUALS(sym->index(state)->to_native(), sym->index());
  }

  void test_to_str() {
    Symbol* sym = state->symbols.lookup(state, "blah");
    String* str = sym->to_str(state);

    TS_ASSERT(!strncmp("blah", str->byte_address(state), 4));
  }
};
