#include "vm/test/test.hpp"

#include "builtin/symbol.hpp"

class TestSymbol : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_index() {
    Symbol* sym = state->symbol("blah");

    TS_ASSERT_EQUALS(sym->index(state)->to_native(), sym->index());
  }

  void test_to_str() {
    Symbol* sym = state->symbol("blah");
    String* str = sym->to_str(state);
    TS_ASSERT(!strncmp("blah", str->c_str(state), 4));
  }

  void test_all_symbols() {
    Array* symbols = Symbol::all_symbols(state);

    TS_ASSERT(kind_of<Array>(symbols));
    TS_ASSERT(state->shared.symbols.size() > 0);
  }
};
