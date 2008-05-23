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
    TS_ASSERT(!str->CanStoreIvars);
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
    str->to_sym(state);
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
};
