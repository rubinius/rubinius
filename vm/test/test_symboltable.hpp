#include "vm/test/test.hpp"

#include "symboltable.hpp"
#include "configuration.hpp"
#include "builtin/array.hpp"
#include "version.h"

#include <algorithm>
#include <vector>
#include <string.h>

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
    Object* sym = symbols->lookup(state, std::string("unique"));
    TS_ASSERT(sym->symbol_p());

    Object* sym2 = symbols->lookup(state, std::string("unique"));
    TS_ASSERT_EQUALS(sym, sym2);
  }

  void test_lookup_with_empty_string() {
    String* str = String::create(state, "");

    if(LANGUAGE_18_ENABLED) {
      TS_ASSERT_THROWS_ASSERT(symbols->lookup(state, str), const RubyException &e,
                              TS_ASSERT(Exception::argument_error_p(state, e.exception)));
    } else {
      Symbol* sym = symbols->lookup(state, str);
      String* str2 = symbols->lookup_string(state, sym);
      TS_ASSERT_EQUALS(cTrue, str->equal(state, str2));
    }

  }

  void test_lookup_with_null_character_in_string() {
    String* str1 = String::create(state, "\0", 1);
    String* str2 = String::create(state, "\0\0", 2);

    if(LANGUAGE_18_ENABLED) {
      TS_ASSERT_THROWS_ASSERT(symbols->lookup(state, str1), const RubyException &e,
                              TS_ASSERT(Exception::argument_error_p(state, e.exception)));
      TS_ASSERT_THROWS_ASSERT(symbols->lookup(state, str2), const RubyException &e,
                              TS_ASSERT(Exception::argument_error_p(state, e.exception)));
    } else {
      Symbol* sym1 = symbols->lookup(state, str1);
      Symbol* sym2 = symbols->lookup(state, str2);

      TS_ASSERT(sym1 != sym2);

      String* str3 = symbols->lookup_string(state, sym1);
      String* str4 = symbols->lookup_string(state, sym2);

      TS_ASSERT_EQUALS(cTrue, str1->equal(state, str3));
      TS_ASSERT_EQUALS(cTrue, str2->equal(state, str4));
      TS_ASSERT_EQUALS(symbols->size(), 2U);
    }
  }

  void notest_lookup_colliding_hash() {
    Object* sym;
    Object* sym2;
    const char* str = "__uint_fast64_t";
    const char* str2 = "TkIF_MOD";

    TS_ASSERT_EQUALS(String::hash_str((unsigned char*)str, strlen(str), 0),
                     String::hash_str((unsigned char*)str2, strlen(str2), 0));

    sym  = symbols->lookup(state, std::string(str));
    sym2 = symbols->lookup(state, std::string(str2));

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

  void test_lookup_string() {
    Symbol* sym = symbols->lookup(state, "circle");
    String* str = symbols->lookup_string(state, sym);

    TS_ASSERT(!strncmp("circle", str->c_str(state), 6));
  }

  void test_lookup_nil() {
    TS_ASSERT_THROWS_ASSERT(symbols->lookup(state, nil<String>()),
                            const RubyException &e,
                            TS_ASSERT(Exception::argument_error_p(state, e.exception)));
  }

  void test_lookup_string_nil() {
    TS_ASSERT_THROWS_ASSERT(symbols->lookup_string(state, nil<Symbol>()),
                            const RubyException &e,
                            TS_ASSERT(Exception::argument_error_p(state, e.exception)));
  }

  void test_lookup_debug_str() {
    if(LANGUAGE_18_ENABLED) {
      String* str1 = String::create(state, "blah\1wha", 8);
      Symbol* sym1 = symbols->lookup(state, str1);
      std::string symstr1 = symbols->lookup_debug_string(sym1);
      TS_ASSERT(!strncmp("blah\\x01wha", symstr1.c_str(), 11));
    } else {
      String* str1 = String::create(state, "blah\0wha", 8);
      Symbol* sym1 = symbols->lookup(state, str1);
      std::string symstr1 = symbols->lookup_debug_string(sym1);
      TS_ASSERT(!strncmp("blah\\x00wha", symstr1.c_str(), 11));
    }

    String* str2 = String::create(state, "blahéwha", 8);
    Symbol* sym2 = symbols->lookup(state, str2);
    std::string symstr2 = symbols->lookup_debug_string(sym2);
    TS_ASSERT(!strncmp("blah\\xc3\\xa9wha", symstr2.c_str(), 11));
  }

  void test_size() {
    symbols->lookup(state, "uno");
    symbols->lookup(state, "dos");
    symbols->lookup(state, "tres");

    TS_ASSERT_EQUALS(symbols->size(), 3U);
  }

  void test_many_symbols() {
    size_t size = symbols->size();

    for(size_t i = 0; i < 100; i++) {
      std::ostringstream stream;
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
    TS_ASSERT_EQUALS(ary->size(), 6);

    for(native_int i = 0; i < ary->size(); i++) {
      TS_ASSERT(std::find(syms.begin(), syms.end(), ary->get(state, i)) != syms.end());
    }
  }
};
