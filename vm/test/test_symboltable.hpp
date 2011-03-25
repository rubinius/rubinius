#include "vm/test/test.hpp"

#include "symboltable.hpp"
#include "builtin/array.hpp"

#include <algorithm>
#include <vector>

class TestSymbolTable : public CxxTest::TestSuite, public VMTest {
public:

  SymbolTable* symbols;

  void setUp() {
    create();
    symbols = new SymbolTable();
  }

  void tearDown() {
    destroy();
  }

  void test_lookup_with_c_str() {
    Object* sym = symbols->lookup(state, "unique");
    TS_ASSERT(sym->symbol_p());

    Object* sym2 = symbols->lookup(state, "unique");
    TS_ASSERT_EQUALS(sym, sym2);
  }

  void test_lookup_with_string_containing_null() {
    String* str = String::create(state, "abxdc");
    str->byte_address()[2] = 0;

    TS_ASSERT_THROWS_ASSERT(symbols->lookup(state, str), const RubyException &e,
                            TS_ASSERT(Exception::argument_error_p(state, e.exception)));
  }

  void test_lookup_with_empty_string() {
    String* str = String::create(state, "");

    TS_ASSERT_THROWS_ASSERT(symbols->lookup(state, str), const RubyException &e,
                            TS_ASSERT(Exception::argument_error_p(state, e.exception)));
  }

  /* Uncomment when 2 strings are found that have colliding hash values
  void notest_lookup_colliding_hash() {
    Object* sym;
    Object* sym2;
    const char* str = "__uint_fast64_t";
    const char* str2 = "TkIF_MOD";

    TS_ASSERT_EQUALS(String::hash_str((unsigned char*)str, std::strlen(str)),
                     String::hash_str((unsigned char*)str2, std::strlen(str2)));

    sym  = symbols->lookup(state, str);
    sym2 = symbols->lookup(state, str2);

    TS_ASSERT(sym != sym2);
  }

  void notest_lookup_colliding_hash2() {
    String* a = String::create(state, "__uint_fast64_t");
    String* b = String::create(state, "TkIF_MOD");

    TS_ASSERT_EQUALS(a->hash_string(state), b->hash_string(state));

    Object* sym;
    Object* sym2;

    sym  = symbols->lookup(state, a);
    sym2 = symbols->lookup(state, b);

    TS_ASSERT(sym != sym2);
  }
  */

  void test_lookup_string() {
    Symbol* sym = symbols->lookup(state, "circle");
    String* str = symbols->lookup_string(state, sym);

    TS_ASSERT(!strncmp("circle", str->c_str(state), 6));
  }

  void test_lookup_nil() {
    TS_ASSERT_THROWS_ASSERT(symbols->lookup(state, reinterpret_cast<String*>(Qnil)),
                            const RubyException &e,
                            TS_ASSERT(Exception::argument_error_p(state, e.exception)));
  }

  void test_lookup_string_nil() {
    TS_ASSERT_THROWS_ASSERT(symbols->lookup_string(state, reinterpret_cast<Symbol*>(Qnil)),
                            const RubyException &e,
                            TS_ASSERT(Exception::argument_error_p(state, e.exception)));
  }

  void test_lookup_cstring_nil() {
    TS_ASSERT_THROWS_ASSERT(symbols->lookup_cstring(state, reinterpret_cast<Symbol*>(Qnil)),
                            const RubyException &e,
                            TS_ASSERT(Exception::argument_error_p(state, e.exception)));
  }

  void test_size() {
    symbols->lookup(state, "uno");
    symbols->lookup(state, "dos");
    symbols->lookup(state, "tres");

    TS_ASSERT_EQUALS(symbols->size(), 3U)
  }

  void test_many_symbols() {
    size_t size = symbols->size();

    for(size_t i = 0; i < 100; i++) {
      std::stringstream stream;
      stream << "sym" << i;
      symbols->lookup(state, stream.str().c_str());
    }

    TS_ASSERT(symbols->size() > size);
  }

  void test_all_as_array() {
    std::vector<Symbol*> syms;

    syms.push_back(symbols->lookup(state, "__uint_fast64_t"));
    syms.push_back(symbols->lookup(state, "ponies"));
    syms.push_back(symbols->lookup(state, "cupcakes"));
    syms.push_back(symbols->lookup(state, "TkIF_MOD"));
    syms.push_back(symbols->lookup(state, "turtles"));
    syms.push_back(symbols->lookup(state, "starts"));

    Array* ary = symbols->all_as_array(state);
    TS_ASSERT_EQUALS(ary->size(), 6U);

    for(size_t i = 0; i < ary->size(); i++) {
      TS_ASSERT(std::find(syms.begin(), syms.end(), ary->get(state, i)) != syms.end());
    }
  }
};
