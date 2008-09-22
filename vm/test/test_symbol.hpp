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

  void test_symbol_fields() {
    TS_ASSERT_EQUALS(0, Symbol::fields);
  }

  void test_index() {
    Symbol* sym = state->symbol("blah");

    TS_ASSERT_EQUALS(sym->index(state)->to_native(), sym->index());
  }

  void test_to_str() {
    Symbol* sym = state->symbol("blah");
    String* str = sym->to_str(state);

    TS_ASSERT(!strncmp("blah", str->c_str(), 4));
  }

  void test_all_symbols() {
    Array* symbols = Symbol::all_symbols(state);

    TS_ASSERT(kind_of<Array>(symbols));
    TS_ASSERT(state->symbols.size() > 0);
  }
};
