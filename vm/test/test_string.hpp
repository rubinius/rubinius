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

  void test_tr_expand() {
    String* s = String::create(state, "a-g");
    TS_ASSERT_EQUALS(s->tr_expand(state, Qnil), Fixnum::from(7));
    TS_ASSERT_SAME_DATA(s->data->bytes, "abcdefg", 7);
  }

  void test_tr_expand_range() {
    String* s = String::create(state, "abc-egh");
    TS_ASSERT_EQUALS(s->tr_expand(state, Qnil), Fixnum::from(7));
    TS_ASSERT_SAME_DATA(s->data->bytes, "abcdegh", 7);
  }

  void test_tr_expand_duplicate_chars_last_position() {
    String* s = String::create(state, "a-cabdage");
    TS_ASSERT_EQUALS(s->tr_expand(state, Qnil), Fixnum::from(9));
    TS_ASSERT_SAME_DATA(s->data->bytes, "cbdage", 6);
  }

  void test_tr_expand_leading_carat() {
    String* s = String::create(state, "^");
    TS_ASSERT_EQUALS(s->tr_expand(state, Qnil), Fixnum::from(1));
    TS_ASSERT_SAME_DATA(s->data->bytes, "^", 1);

    s = String::create(state, "^a-c");
    TS_ASSERT_EQUALS(s->tr_expand(state, Qnil), Fixnum::from(3));
    TS_ASSERT_SAME_DATA(s->data->bytes, "abc", 3);
  }

  void test_tr_expand_limit_processing() {
    String* s = String::create(state, "a-h");
    FIXNUM five = Fixnum::from(5);
    TS_ASSERT_EQUALS(s->tr_expand(state, five), five);
    TS_ASSERT_SAME_DATA(s->data->bytes, "abcde", 5);

    s = String::create(state, "abc-ga-i");
    FIXNUM ten = Fixnum::from(10);
    TS_ASSERT_EQUALS(s->tr_expand(state, ten), ten);
    TS_ASSERT_SAME_DATA(s->data->bytes, "defgabc", 7);

    s = String::create(state, "abc-ga-i");
    FIXNUM three = Fixnum::from(3);
    TS_ASSERT_EQUALS(s->tr_expand(state, three), three);
    TS_ASSERT_SAME_DATA(s->data->bytes, "abc", 3);

    s = String::create(state, "^abcde");
    FIXNUM four = Fixnum::from(4);
    TS_ASSERT_EQUALS(s->tr_expand(state, four), four);
    TS_ASSERT_SAME_DATA(s->data->bytes, "abcd", 4);
  }

  void test_copy_from() {
    String* a = String::create(state, "abcdefghijkl");
    String* b = String::create(state, "ihgfed");
    a->copy_from(state, b, Fixnum::from(1), Fixnum::from(4), Fixnum::from(4));
    TS_ASSERT_SAME_DATA(a->data->bytes, "abcdhgfeijkl", 12);
  }

  void test_copy_from_limit_copy_from_size() {
    String* a = String::create(state, "xxxxxx");
    String* b = String::create(state, "yy");
    a->copy_from(state, b, Fixnum::from(0), Fixnum::from(3), Fixnum::from(2));
    TS_ASSERT_SAME_DATA(a->data->bytes, "xxyyxx", 6);
  }

  void test_copy_from_start_not_less_than_zero() {
    String* a = String::create(state, "xxx");
    String* b = String::create(state, "yy");
    a->copy_from(state, b, Fixnum::from(-1), Fixnum::from(2), Fixnum::from(1));
    TS_ASSERT_SAME_DATA(a->data->bytes, "xyy", 3);
  }

  void test_copy_from_start_not_greater_than_size() {
    String* a = String::create(state, "xxx");
    String* b = String::create(state, "yy");
    a->copy_from(state, b, Fixnum::from(2), Fixnum::from(2), Fixnum::from(0));
    TS_ASSERT_SAME_DATA(a->data->bytes, "xxx", 3);
  }

  void test_copy_from_limit_copy_to_size() {
    String* a = String::create(state, "xx");
    String* b = String::create(state, "yyyy");
    a->copy_from(state, b, Fixnum::from(0), Fixnum::from(4), Fixnum::from(0));
    TS_ASSERT_SAME_DATA(a->data->bytes, "yy", 2);
  }

  void test_copy_from_only_copy_if_dest_is_within_bounds() {
    String* a = String::create(state, "xx");
    String* b = String::create(state, "yy");
    a->copy_from(state, b, Fixnum::from(0), Fixnum::from(2), Fixnum::from(2));
    TS_ASSERT_SAME_DATA(a->data->bytes, "xx", 2);
  }

  void test_copy_from_dest_start_not_less_than_zero() {
    String* a = String::create(state, "xx");
    String* b = String::create(state, "yy");
    a->copy_from(state, b, Fixnum::from(0), Fixnum::from(2), Fixnum::from(-1));
    TS_ASSERT_SAME_DATA(a->data->bytes, "yy", 2);
  }
};
