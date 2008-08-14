#include "objects.hpp"
#include "vm.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestString : public CxxTest::TestSuite {
  public:

  VM *state;
  String *str;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    TS_ASSERT(state->globals.string->has_ivars->false_p());
    str = String::create(state, "blah");
    TS_ASSERT_EQUALS(str->size(state), 4U);
  }

  void test_create2() {
    str = String::create(state, "blah", 2);
    TS_ASSERT_EQUALS(str->size(state), 2U);
  }

  void test_hash_string() {
    str = String::create(state, "blah");
    hashval hash = str->hash_string(state);
    TS_ASSERT(hash > 0);

    hashval again = str->hash_string(state);

    TS_ASSERT_EQUALS(hash, again);
  }

  void test_to_sym() {
    str = String::create(state, "blah");
    OBJECT sym = str->to_sym(state);

    TS_ASSERT(sym->symbol_p());
  }

  void test_string_dup() {
    str = String::create(state, "blah");
    String* str2 = str->string_dup(state);

    TS_ASSERT_EQUALS(str->shared, Qtrue);
    TS_ASSERT_EQUALS(str2->shared, Qtrue);

    TS_ASSERT_EQUALS(str->data, str2->data);
  }

  void test_unshare() {
    str = String::create(state, "blah");
    String* str2 = str->string_dup(state);

    str->unshare(state);

    TS_ASSERT(str->data != str2->data);
    TS_ASSERT_EQUALS(std::string("blah"), (char*)*str);
  }

  void test_append() {
    str = String::create(state, "blah");
    str->hash_string(state);
    TS_ASSERT(str->hash != Qnil);

    str->append(state, String::create(state, " foo"));
    TS_ASSERT_EQUALS(str->hash, Qnil);

    TS_ASSERT_EQUALS(std::string("blah foo"), (char*)*str);
  }

  void test_append_with_charstar() {
    str = String::create(state, "blah");
    str->append(state, " foo");

    TS_ASSERT_EQUALS(std::string("blah foo"), (char*)*str);
  }

  void test_add() {
    str = String::create(state, "blah");
    String* str2 = str->add(state, String::create(state, " foo"));

    TS_ASSERT_EQUALS(std::string("blah foo"), (char*)*str2);
    TS_ASSERT_EQUALS(std::string("blah"), (char*)*str);

  }

  void test_equal() {
    String* str1 = String::create(state, "a_string");
    String* str2 = String::create(state, "a_string");
    String* str3 = String::create(state, "another_string");

    TS_ASSERT_EQUALS(str1->equal(state, str2), Qtrue);
    TS_ASSERT_EQUALS(str1->equal(state, str3), Qfalse);
  }

  void test_to_double() {
    str = String::create(state, "2.10");
    double val = 2.10;
    double ret = str->to_double(state);
    TS_ASSERT_EQUALS(val, ret);
  }

  void test_to_f() {
    str = String::create(state, "6.50");
    Float* val = Float::create(state, 6.50);
    Float* ret = str->to_f(state);
    TS_ASSERT(val->equal(state, ret));
  }

  void test_apply_and() {
    String* expected = String::create(state, "\001\000\000");

    String* str1 = String::create(state, "\001\000\001");
    String* str2 = String::create(state, "\001\001\000");
    str1->apply_and(state, str2);
    TS_ASSERT(expected->equal(state, str1));

    String* str3 = String::create(state, "\001\001\001");
    String* str4 = String::create(state, "\008\000\000\000\000");
    str3->apply_and(state, str4);
    TS_ASSERT(expected->equal(state, str3));
  }
};
