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

  void test_lookup() {
    OBJECT sym = state->globals.symbols->lookup(state, "blah", 4);
    TS_ASSERT(sym->symbol_p());

    OBJECT sym2 = state->globals.symbols->lookup(state, "blah", 4);
    TS_ASSERT_EQUALS(sym, sym2);
  }

  void test_lookup_colliding_hash() {
    OBJECT sym, sym2;

    sym  = state->globals.symbols->lookup(state, "__uint_fast64_t");
    sym2 = state->globals.symbols->lookup(state, "TkIF_MOD");

    TS_ASSERT(sym != sym2);
  }

  void test_lookup_colliding_hash2() {
    String* a = String::create(state, "__uint_fast64_t");
    String* b = String::create(state, "TkIF_MOD");

    TS_ASSERT_EQUALS(a->hash_string(state), b->hash_string(state));

    OBJECT sym, sym2;

    sym  = state->globals.symbols->lookup(state, a);
    sym2 = state->globals.symbols->lookup(state, b);

    TS_ASSERT(sym != sym2);
  }

  void test_lookup_enlarges_table() {
    size_t size = state->globals.symbols->symbols->field_count;

    for(size_t i = 0; i < 100; i++) {
      std::stringstream stream;
      stream << "sym" << i;
      state->globals.symbols->lookup(state, stream.str().c_str());
    }

    TS_ASSERT(state->globals.symbols->symbols->field_count > size);
  }

  void test_find_string() {
    Symbol* sym = (Symbol*)state->globals.symbols->lookup(state, "blah");

    String* str = (String*)state->globals.symbols->find_string(state, sym);

    TS_ASSERT(!strncmp("blah", str->byte_address(state), 4));
  }

  void test_index() {
    Symbol* sym = state->globals.symbols->lookup(state, "blah", 4);

    TS_ASSERT_EQUALS(sym->index(state)->n2i(), sym->index());
  }

  void test_to_str() {
    Symbol* sym = (Symbol*)state->globals.symbols->lookup(state, "blah");

    String* str = (String*)sym->to_str(state);

    TS_ASSERT(!strncmp("blah", str->byte_address(state), 4));

  }
};
