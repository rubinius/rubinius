#include "machine/test/test.hpp"

#include "vm.hpp"
#include "state.hpp"
#include "memory.hpp"
#include "memory/debug.hpp"

#include <cxxtest/TestSuite.h>

#include <map>
#include <vector>

using namespace rubinius;

class TestVM : public CxxTest::TestSuite, public VMTest {
  public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_symbol_given_cstr() {
    Symbol* sym1 = state->symbol("blah");
    Symbol* sym2 = state->symbol("blah");

    TS_ASSERT_EQUALS(sym1, sym2);
  }

  void test_symbol_given_string() {
    String* str1 = String::create(state, "symbolic");
    String* str2 = String::create(state, "symbolic");

    Symbol* sym1 = state->symbol(str1);
    Symbol* sym2 = state->symbol(str2);

    TS_ASSERT_EQUALS(sym1, sym2);
  }
};
