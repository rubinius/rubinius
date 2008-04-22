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

  void test_new_object_uses_field_count_from_class() {
    Class* cls = state->new_class("Blah", G(object), 3);

    OBJECT blah = state->new_object(cls);

    TS_ASSERT_EQUALS(blah->field_count, 3);
  }

  void test_globals() {
    TS_ASSERT_EQUALS(state->globals.roots.size(), 117);
  }
};
