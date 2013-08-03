#include "vm/test/test.hpp"

#include "builtin/encoding.hpp"
#include "builtin/string.hpp"
#include "version.h"

class TestString : public CxxTest::TestSuite, public VMTest {
public:

  String *str;

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_create_with_size() {
    str = String::create(state, Fixnum::from(4));
    TS_ASSERT_EQUALS(str->byte_size(), 4);
    TS_ASSERT_EQUALS(str->data()->size(), 8);
  }

  void test_create() {
    str = String::create(state, "blah");
    TS_ASSERT_EQUALS(str->byte_size(), 4);
  }

  void test_create_with_substring() {
    str = String::create(state, "blah", 2);
    TS_ASSERT_EQUALS(str->byte_size(), 2);
    TS_ASSERT_SAME_DATA("bl\0", str->c_str(state), 3);
  }

  void test_create_with_null_and_zero_count() {
    str = String::create(state, NULL, 0);
    TS_ASSERT_EQUALS(str->byte_size(), 0);
    TS_ASSERT_EQUALS(str->byte_address()[0], 0);
  }

  void test_hash_string() {
    str = String::create(state, "blah");
    hashval hash = str->hash_string(state);
    TS_ASSERT(hash > 0);

    hashval again = str->hash_string(state);

    TS_ASSERT_EQUALS(hash, again);

    String* another_str = String::create(state, "blah");
    hashval another_hash = another_str->hash_string(state);

    TS_ASSERT_EQUALS(hash, another_hash);
  }

  void test_to_sym() {
    str = String::create(state, "blah");
    Object* sym = str->to_sym(state);

    TS_ASSERT(sym->symbol_p());
  }

  void test_string_dup() {
    Encoding* enc = Encoding::ascii8bit_encoding(state);
    str = String::create(state, "blah");
    str->encoding(state, enc);
    str->valid_encoding(state, cTrue);
    str->ascii_only(state, cFalse);

    String* str2 = str->string_dup(state);

    TS_ASSERT_EQUALS(str->shared(), cTrue);
    TS_ASSERT_EQUALS(str2->shared(), cTrue);

    TS_ASSERT_EQUALS(str->data(), str2->data());

    TS_ASSERT_EQUALS(enc, str2->encoding());
    TS_ASSERT_EQUALS(cFalse, str2->ascii_only());
    TS_ASSERT_EQUALS(cTrue, str2->valid_encoding());
  }

  void test_unshare() {
    str = String::create(state, "blah");
    String* str2 = str->string_dup(state);

    str->unshare(state);

    TS_ASSERT(str->data() != str2->data());
    TS_ASSERT_EQUALS(std::string("blah"), str->c_str(state));
  }

  void test_cstr() {
    // We need to setup a string that uses a ByteArray
    // to full capacity in order to test c_str() resizing
    str = String::create(state, "zzzzzzz");
    str->num_bytes(state, Fixnum::from(8));
    str->byte_address()[7] = 'z';

    // We should still have a data backend of 8 bytes
    // which are completely filled by all 'z' characters
    TS_ASSERT_EQUALS(str->data()->size(), 8);
    TS_ASSERT(!memcmp("zzzzzzzz", str->byte_address(), 8));

    const char* ptr = str->c_str(state);
    // Sizes are aligned so they end up as 16 bytes used
    TS_ASSERT(str->data()->size() > 8);
    TS_ASSERT(!memcmp("zzzzzzzz\0", ptr, 9));
    TS_ASSERT(!memcmp("zzzzzzzz\0", str->byte_address(), 9));
  }

  void test_append() {
    String* s1 = String::create(state, "omote ");

    s1->append(state, "u\0ra");

    TS_ASSERT_EQUALS(7, s1->byte_size());
    TS_ASSERT(s1->byte_size() < s1->data()->size());
    TS_ASSERT_SAME_DATA("omote u\0", s1->byte_address(), 8);
  }

  void test_append__with_length() {
    String* str = String::create(state, "foo");

    str->append(state, "bar", 1U);

    TS_ASSERT_EQUALS(4, str->byte_size());
    TS_ASSERT(str->byte_size() < str->data()->size());
    TS_ASSERT_SAME_DATA("foob\0", str->byte_address(), 5);
  }

  void test_append_prim() {
    String* s1 = String::create(state, "omote ");
    String* s2 = String::create(state, "u\0ra", 4U);

    s1->hash_string(state);
    TS_ASSERT(!s1->hash_value()->nil_p());

    s1->append(state, s2);
    TS_ASSERT_EQUALS(s1->hash_value(), cNil);

    TS_ASSERT_EQUALS(10, s1->byte_size());
    TS_ASSERT(s1->byte_size() < s1->data()->size());
    TS_ASSERT_SAME_DATA("omote u\0ra\0", s1->byte_address(), 11);
  }

  void test_append_with_charstar() {
    str = String::create(state, "blah");
    str->append(state, " foo");

    TS_ASSERT_EQUALS(8, str->byte_size())
    TS_ASSERT_SAME_DATA("blah foo\0", str->byte_address(), 9);
  }

  void test_add() {
    str = String::create(state, "blah");
    String* str2 = str->add(state, String::create(state, " foo"));

    TS_ASSERT_EQUALS(std::string("blah foo"), str2->c_str(state));
    TS_ASSERT_EQUALS(std::string("blah"), str->c_str(state));
  }

  void test_equal() {
    String* str1 = String::create(state, "a_string");
    String* str2 = String::create(state, "a_string");
    String* str3 = String::create(state, "another_string");

    TS_ASSERT_EQUALS(str1->equal(state, str2), cTrue);
    TS_ASSERT_EQUALS(str1->equal(state, str3), cFalse);
  }

  void test_to_f() {
    str = String::create(state, "0");
    Float* val = Float::create(state, 0.0);
    Float* ret = str->to_f(state);
    TS_ASSERT(val->equal(state, ret));

    str = String::create(state, "6.50");
    val = Float::create(state, 6.50);
    ret = str->to_f(state);
    TS_ASSERT(val->equal(state, ret));

    str = String::create(state, "6.50 43.2");
    val = Float::create(state, 6.50);
    ret = str->to_f(state, cFalse);
    TS_ASSERT(val->equal(state, ret));

    str = String::create(state, "NaN");
    ret = str->to_f(state, cTrue);
    TS_ASSERT_EQUALS(ret, cNil);
    val = Float::create(state, 0.0);
    ret = str->to_f(state, cFalse);
    TS_ASSERT(val->equal(state, ret));

    str = String::create(state, "-Infinity");
    ret = str->to_f(state, cTrue);
    TS_ASSERT_EQUALS(ret, cNil);
    val = Float::create(state, 0.0);
    ret = str->to_f(state, cFalse);
    TS_ASSERT(val->equal(state, ret));

    str = String::create(state, "Infinity");
    ret = str->to_f(state, cTrue);
    TS_ASSERT_EQUALS(ret, cNil);
    val = Float::create(state, 0.0);
    ret = str->to_f(state, cFalse);
    TS_ASSERT(val->equal(state, ret));

    str = String::create(state, "-Inf");
    ret = str->to_f(state, cTrue);
    TS_ASSERT_EQUALS(ret, cNil);
    val = Float::create(state, 0.0);
    ret = str->to_f(state, cFalse);
    TS_ASSERT(val->equal(state, ret));

    str = String::create(state, "Inf");
    ret = str->to_f(state, cTrue);
    TS_ASSERT_EQUALS(ret, cNil);
    val = Float::create(state, 0.0);
    ret = str->to_f(state, cFalse);
    TS_ASSERT(val->equal(state, ret));

    str = String::create(state, "1.23e4");
    val = Float::create(state, 12300.0);
    ret = str->to_f(state);
    TS_ASSERT(val->equal(state, ret));
    str = String::create(state, "1.23E4");
    ret = str->to_f(state);
    TS_ASSERT(val->equal(state, ret));

    str = String::create(state, " _6.50_ ");
    val = Float::create(state, 6.50);
    ret = str->to_f(state, cFalse);
    TS_ASSERT(val->equal(state, ret));

    str = String::create(state, " _6.50");
    ret = str->to_f(state);
    TS_ASSERT_EQUALS(ret, cNil);

    str = String::create(state, "6.50_");
    ret = str->to_f(state);
    TS_ASSERT_EQUALS(ret, cNil);

    str = String::create(state, "0x3.4");
    ret = str->to_f(state, cFalse);
    val = Float::create(state, 0.0);
    TS_ASSERT(val->equal(state, ret));
    ret = str->to_f(state, cFalse);
    TS_ASSERT(val->equal(state, ret));
  }

  void test_to_i() {
    str = String::create(state, "0");
    Integer* val = str->to_i(state);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 0);

    str = String::create(state, "0");
    val = str->to_i(state, Fixnum::from(10), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 0);

    str = String::create(state, "3");
    val = str->to_i(state);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 3);

    str = String::create(state, "  3  ");
    val = str->to_i(state);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 3);

    str = String::create(state, "-3");
    val = str->to_i(state);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), -3);

    str = String::create(state, "   +3");
    val = str->to_i(state, Fixnum::from(10), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 3);
    val = str->to_i(state, Fixnum::from(0), cTrue);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 3);

    str = String::create(state, " 3F ");
    val = str->to_i(state);
    TS_ASSERT_EQUALS(val, cNil);

    str = String::create(state, " F ");
    val = str->to_i(state);
    TS_ASSERT_EQUALS(val, cNil);

    str = String::create(state, "garbage");
    val = str->to_i(state);
    TS_ASSERT_EQUALS(val, cNil);

    str = String::create(state, "garbage");
    val = str->to_i(state, Fixnum::from(0), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 0);

    str = String::create(state, "0xa");
    val = str->to_i(state);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 0xa);

    str = String::create(state, "0xaq");
    val = str->to_i(state);
    TS_ASSERT_EQUALS(val, cNil);

    str = String::create(state, "0b101");
    val = str->to_i(state);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 5);

    str = String::create(state, "0777");
    val = str->to_i(state);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 511);

    str = String::create(state, "08");
    val = str->to_i(state, Fixnum::from(0), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 0);

    val = str->to_i(state, Fixnum::from(0), cTrue);
    TS_ASSERT_EQUALS(val, cNil);

    val = str->to_i(state, Fixnum::from(10), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 8)

    val = str->to_i(state, Fixnum::from(10), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 8)

    str = String::create(state, "46234326");
    val = str->to_i(state);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 46234326);

    str = String::create(state, "9223372036854775807");
    val = str->to_i(state);
    TS_ASSERT(kind_of<Bignum>(val));
    Bignum *big;

    big = Bignum::from(state, 9223372036854775807LL);
    big = as<Bignum>(big->mul(state, big));

    TS_ASSERT(as<Bignum>(val)->equal(state, big));

    str = String::create(state, "0b11");
    val = str->to_i(state, Fixnum::from(10), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 0);

    str = String::create(state, "0b11");
    val = str->to_i(state, Fixnum::from(2), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 3);

    str = String::create(state, "0b11");
    val = str->to_i(state, Fixnum::from(2), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 3);

    str = String::create(state, "0b11");
    val = str->to_i(state, Fixnum::from(10), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 0);

    str = String::create(state, "0 1");
    val = str->to_i(state, Fixnum::from(2), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 0);

    str = String::create(state, "0_1");
    val = str->to_i(state, Fixnum::from(2), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 1);

    str = String::create(state, "1_1");
    val = str->to_i(state, Fixnum::from(10), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 11);

    str = String::create(state, "1_2");
    val = str->to_i(state, Fixnum::from(10), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 12);

    str = String::create(state, "1_");
    val = str->to_i(state, Fixnum::from(10), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 1);

    // Make sure we support up to base 36
    str = String::create(state, "a8q8a");
    val = str->to_i(state, Fixnum::from(36), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 17203402);

    // Boundry conditions
    str = String::create(state, "a8q8a");
    val = str->to_i(state, Fixnum::from(37), cFalse);
    TS_ASSERT_EQUALS(val, cNil);

    str = String::create(state, "_12");
    val = str->to_i(state, Fixnum::from(10), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    if(LANGUAGE_18_ENABLED) {
      TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 12);
    } else {
      TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), 0);
    }

    str = String::create(state, "-45q");
    val = str->to_i(state, Fixnum::from(10), cFalse);
    TS_ASSERT(kind_of<Fixnum>(val));
    TS_ASSERT_EQUALS(as<Fixnum>(val)->to_native(), -45);

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
    TS_ASSERT_EQUALS(s->tr_expand(state, cNil, cTrue), Fixnum::from(7));
    TS_ASSERT_SAME_DATA(s->byte_address(), "abcdefg", 7);
  }

  void test_tr_expand_range() {
    String* s = String::create(state, "abc-egh");
    TS_ASSERT_EQUALS(s->tr_expand(state, cNil, cTrue), Fixnum::from(7));
    TS_ASSERT_SAME_DATA(s->byte_address(), "abcdegh", 7);
  }

  void test_tr_expand_duplicate_chars_last_position() {
    String* s = String::create(state, "a-cabdage");
    TS_ASSERT_EQUALS(s->tr_expand(state, cNil, cTrue), Fixnum::from(9));
    TS_ASSERT_SAME_DATA(s->byte_address(), "abcabdage", 9);
  }

  void test_tr_expand_leading_carat() {
    String* s = String::create(state, "^");
    TS_ASSERT_EQUALS(s->tr_expand(state, cNil, cTrue), Fixnum::from(1));
    TS_ASSERT_SAME_DATA(s->byte_address(), "^", 1);

    s = String::create(state, "^a-c");
    TS_ASSERT_EQUALS(s->tr_expand(state, cNil, cTrue), Fixnum::from(4));
    TS_ASSERT_SAME_DATA(s->byte_address(), "^abc", 4);
  }

  void test_tr_expand_limit_processing() {
    String* s = String::create(state, "a-h");
    Fixnum* five = Fixnum::from(5);
    TS_ASSERT_EQUALS(s->tr_expand(state, five, cTrue), five);
    TS_ASSERT_SAME_DATA(s->byte_address(), "abcde", 5);

    s = String::create(state, "abc-ga-i");
    Fixnum* ten = Fixnum::from(10);
    TS_ASSERT_EQUALS(s->tr_expand(state, ten, cTrue), ten);
    TS_ASSERT_SAME_DATA(s->byte_address(), "abcdefgabc", 10);

    s = String::create(state, "abc-ga-i");
    Fixnum* three = Fixnum::from(3);
    TS_ASSERT_EQUALS(s->tr_expand(state, three, cTrue), three);
    TS_ASSERT_SAME_DATA(s->byte_address(), "abc", 3);

    s = String::create(state, "^abcde");
    Fixnum* four = Fixnum::from(4);
    TS_ASSERT_EQUALS(s->tr_expand(state, four, cTrue), four);
    TS_ASSERT_SAME_DATA(s->byte_address(), "^abc", 4);
  }

  void test_tr_expand_all() {
    const char* expected =
      "\000\001\002\003\004\005\006\a\b\t\n\v\f\r\016\017\020\021\022\023\024\025"
      "\026\027\030\031\032\e\034\035\036\037 !\"#$%&'()*+,-./0123456789:;<=>?@AB"
      "CDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\177\200\201\202"
      "\203\204\205\206\207\210\211\212\213\214\215\216\217\220\221\222\223\224\225"
      "\226\227\230\231\232\233\234\235\236\237\240\241\242\243\244\245\246\247\250"
      "\251\252\253\254\255\256\257\260\261\262\263\264\265\266\267\270\271\272\273"
      "\274\275\276\277\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316"
      "\317\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337\340\341"
      "\342\343\344\345\346\347\350\351\352\353\354\355\356\357\360\361\362\363\364"
      "\365\366\367\370\371\372\373\374\375\376\377";

    String* s = String::create(state, "\x00-\xFF", 3);
    TS_ASSERT_EQUALS(Fixnum::from(256), s->tr_expand(state, cNil, cTrue));
    TS_ASSERT_SAME_DATA(expected, s->byte_address(), 256);
  }

  void test_copy_from() {
    String* a = String::create(state, "abcdefghijkl");
    String* b = String::create(state, "ihgfed");
    a->copy_from(state, b, Fixnum::from(1), Fixnum::from(4), Fixnum::from(4));
    TS_ASSERT_SAME_DATA(a->byte_address(), "abcdhgfeijkl", 12);
  }

  void test_copy_from_limit_copy_from_size() {
    String* a = String::create(state, "xxxxxx");
    String* b = String::create(state, "yy");
    a->copy_from(state, b, Fixnum::from(0), Fixnum::from(3), Fixnum::from(2));
    TS_ASSERT_SAME_DATA(a->byte_address(), "xxyyxx", 6);
  }

  void test_copy_from_start_not_less_than_zero() {
    String* a = String::create(state, "xxx");
    String* b = String::create(state, "yy");
    a->copy_from(state, b, Fixnum::from(-1), Fixnum::from(2), Fixnum::from(1));
    TS_ASSERT_SAME_DATA(a->byte_address(), "xyy", 3);
  }

  void test_copy_from_start_not_greater_than_size() {
    String* a = String::create(state, "xxx");
    String* b = String::create(state, "yy");
    a->copy_from(state, b, Fixnum::from(2), Fixnum::from(2), Fixnum::from(0));
    TS_ASSERT_SAME_DATA(a->byte_address(), "xxx", 3);
  }

  void test_copy_from_limit_copy_to_size() {
    String* a = String::create(state, "xx");
    String* b = String::create(state, "yyyy");
    a->copy_from(state, b, Fixnum::from(0), Fixnum::from(4), Fixnum::from(0));
    TS_ASSERT_SAME_DATA(a->byte_address(), "yy", 2);
  }

  void test_copy_from_only_copy_if_dest_is_within_bounds() {
    String* a = String::create(state, "xx");
    String* b = String::create(state, "yy");
    a->copy_from(state, b, Fixnum::from(0), Fixnum::from(2), Fixnum::from(2));
    TS_ASSERT_SAME_DATA(a->byte_address(), "xx", 2);
  }

  void test_copy_from_dest_start_not_less_than_zero() {
    String* a = String::create(state, "xx");
    String* b = String::create(state, "yy");
    a->copy_from(state, b, Fixnum::from(0), Fixnum::from(2), Fixnum::from(-1));
    TS_ASSERT_SAME_DATA(a->byte_address(), "yy", 2);
  }

  void test_compare_substring_less_than() {
    String* a = String::create(state, "abc");
    String* b = String::create(state, "defghi");
    Fixnum* cmp = a->compare_substring(state, b, Fixnum::from(-6), Fixnum::from(3));
    TS_ASSERT_EQUALS(cmp, Fixnum::from(-1));

    a = String::create(state, "def");
    b = String::create(state, "aghib");
    cmp = a->compare_substring(state, b, Fixnum::from(1), Fixnum::from(3));
    TS_ASSERT_EQUALS(cmp, Fixnum::from(-1));
  }

  void test_compare_substring_equal() {
    String* a = String::create(state, "abc");
    String* b = String::create(state, "ababcba");
    Fixnum* cmp = a->compare_substring(state, b, Fixnum::from(2), Fixnum::from(2));
    TS_ASSERT_EQUALS(cmp, Fixnum::from(0));

    a = String::create(state, "bbc");
    b = String::create(state, "abbc");
    cmp = a->compare_substring(state, b, Fixnum::from(-3), Fixnum::from(3));
    TS_ASSERT_EQUALS(cmp, Fixnum::from(0));
  }

  void test_compare_substring_greater_than() {
    String* a = String::create(state, "def");
    String* b = String::create(state, "xyzabc");
    Fixnum* cmp = a->compare_substring(state, b, Fixnum::from(-3), Fixnum::from(3));
    TS_ASSERT_EQUALS(cmp, Fixnum::from(1));

    a = String::create(state, "qrs");
    b = String::create(state, "abc");
    cmp = a->compare_substring(state, b, Fixnum::from(0), Fixnum::from(2));
    TS_ASSERT_EQUALS(cmp, Fixnum::from(1));
  }

  void test_compare_substring_limit_to_self() {
    String* a = String::create(state, "aaa");
    String* b = String::create(state, "aaabbb");
    Fixnum* cmp = a->compare_substring(state, b, Fixnum::from(0), Fixnum::from(5));
    TS_ASSERT_EQUALS(cmp, Fixnum::from(0));
  }

  void test_compare_substring_limit_to_other() {
    String* a = String::create(state, "aaaa");
    String* b = String::create(state, "aaa");
    Fixnum* cmp = a->compare_substring(state, b, Fixnum::from(1), Fixnum::from(3));
    TS_ASSERT_EQUALS(cmp, Fixnum::from(0));
  }

  void test_compare_substring_throws_if_start_beyond_bounds() {
    String* a = String::create(state, "a");
    String* b = String::create(state, "aa");
    TS_ASSERT_THROWS_ASSERT(a->compare_substring(state, b, Fixnum::from(-3), Fixnum::from(1)),
        const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(a->compare_substring(state, b, Fixnum::from(2), Fixnum::from(1)),
        const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_pattern_from_character() {
    String* s = String::pattern(state, G(string), Fixnum::from(10), Fixnum::from(97));
    TS_ASSERT_SAME_DATA(s->byte_address(), "aaaaaaaaaa", 10);
  }

  void test_pattern_from_string() {
    Fixnum* ten = Fixnum::from(10);
    String* s = String::pattern(state, G(string), ten, String::create(state, "b"));
    TS_ASSERT_SAME_DATA(s->byte_address(), "bbbbbbbbbb", 10);

    s = String::pattern(state, G(string), ten, String::create(state, " "));
    TS_ASSERT_SAME_DATA(s->byte_address(), "          ", 10);

    s = String::pattern(state, G(string), ten, String::create(state, "abc"));
    TS_ASSERT_SAME_DATA(s->byte_address(), "abcabcabca", 10);
  }

  void test_crypt() {
    String* str = String::create(state, "nutmeg");
    String* salt = String::create(state, "Mi");
    TS_ASSERT_SAME_DATA(str->crypt(state, salt)->c_str(state), "MiqkFWCm1fNJI", 14);
  }

  void test_c_str() {
    String* str = String::create(state, "blah");
    TS_ASSERT(str->data()->size() > 4);
    TS_ASSERT_EQUALS(str->byte_address()[4], 0);
    TS_ASSERT_EQUALS(str->c_str(state)[4], 0);
    str->byte_address()[4] = '!';

    TS_ASSERT_EQUALS(str->c_str(state)[4], 0);

  }

  void test_from_bytearray() {
    ByteArray* ba = String::create(state, "partial to ruby")->data();
    Fixnum* six = Fixnum::from(6);
    String* s = String::from_bytearray(state, ba, six, six);
    TS_ASSERT_EQUALS(six, s->num_bytes());
    TS_ASSERT_SAME_DATA("l to r", s->c_str(state), 6);
  }
};
