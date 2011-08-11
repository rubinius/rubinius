#include "vm/test/test.hpp"

#include "vm/object_utils.hpp"
#include "builtin/chararray.hpp"
#include "builtin/exception.hpp"
#include "builtin/string.hpp"
#include "primitives.hpp"

class TestCharArray : public CxxTest::TestSuite, public VMTest {
  public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_create() {
    size_t mag = sizeof(Object*);
    CharArray* c;

    c = CharArray::create(state, 0);
    TS_ASSERT_EQUALS(c->size(state)->to_native(), (native_int)0);

    for(size_t i = 1; i <= mag; i++) {
      c = CharArray::create(state, i);
      TS_ASSERT_EQUALS(c->size(state)->to_native(), (native_int)mag);
    }

    for(size_t i = mag + 1; i <= 2 * mag; i++) {
      c = CharArray::create(state, i);
      TS_ASSERT_EQUALS(c->size(state)->to_native(), (native_int)(2 * mag));
    }

    for(size_t i = 5 * mag + 1; i <= 6 * mag; i++) {
      c = CharArray::create(state, i);
      TS_ASSERT_EQUALS(c->size(state)->to_native(), (native_int)(6 * mag));
    }
  }

  void test_allocate() {
    CharArray* c = CharArray::allocate(state, Fixnum::from(5));
    TS_ASSERT_EQUALS(c->size(state)->to_native(), 8);
  }

  void test_size() {
    CharArray* c = CharArray::create(state, 1);
    TS_ASSERT_EQUALS(c->size(state), Fixnum::from(sizeof(Object*)));
  }

  void test_to_chars() {
    String* s = String::create(state, "xy");
    CharArray* c = s->data();
    char* chars = c->to_chars(state, Fixnum::from(2));

    TS_ASSERT_SAME_DATA("xy", chars, 2);
  }

  void test_get_byte() {
    CharArray* c = String::create(state, "xyz")->data();
    TS_ASSERT_EQUALS(c->get_byte(state, Fixnum::from(0)), Fixnum::from('x'));
    TS_ASSERT_EQUALS(c->get_byte(state, Fixnum::from(2)), Fixnum::from('z'));
  }

  void test_get_byte_index_out_of_bounds() {
    CharArray* c = String::create(state, "xyz")->data();
    native_int sz = c->size(state)->to_native();
    TS_ASSERT_THROWS_ASSERT(c->get_byte(state, Fixnum::from(sz)), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(c->get_byte(state, Fixnum::from(sz+1)), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(c->get_byte(state, Fixnum::from(-1)), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_set_byte() {
    CharArray* c = String::create(state, "xyz")->data();
    c->set_byte(state, Fixnum::from(0), Fixnum::from('1'));
    TS_ASSERT_EQUALS(c->get_byte(state, Fixnum::from(0)), Fixnum::from('1'));
    c->set_byte(state, Fixnum::from(2), Fixnum::from('2'));
    TS_ASSERT_EQUALS(c->get_byte(state, Fixnum::from(2)), Fixnum::from('2'));
  }

  void test_set_byte_out_of_bounds() {
    CharArray* c = String::create(state, "xyz")->data();
    native_int sz = c->size(state)->to_native();
    TS_ASSERT_THROWS_ASSERT(c->set_byte(state, Fixnum::from(sz), Fixnum::from('0')),
        const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(c->set_byte(state, Fixnum::from(sz+1), Fixnum::from('0')),
        const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(c->set_byte(state, Fixnum::from(-1), Fixnum::from('0')),
        const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_move_bytes() {
    String* s = String::create(state, "xyzzy");
    CharArray* c = s->data();
    c->move_bytes(state, Fixnum::from(0), Fixnum::from(2), Fixnum::from(3));
    TS_ASSERT_SAME_DATA(c->raw_bytes(), "xyzxy", 5);
  }

  void test_move_bytes_out_of_bounds() {
    CharArray* c = String::create(state, "xyzzy")->data();

    Fixnum* neg = Fixnum::from(-1);
    Fixnum* one = Fixnum::from(1);
    Fixnum* zero = Fixnum::from(0);
    Fixnum* size = c->size(state);

    TS_ASSERT_THROWS_ASSERT(c->move_bytes(state, neg, zero, zero), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(c->move_bytes(state, zero, neg, zero), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(c->move_bytes(state, zero, zero, neg), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));

    TS_ASSERT_THROWS_ASSERT(c->move_bytes(state, zero, size, one), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(c->move_bytes(state, one, size, zero), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_fetch_bytes() {
    String* s = String::create(state, "xyzzy");
    CharArray* c = s->data();
    CharArray* ca = c->fetch_bytes(state, Fixnum::from(1), Fixnum::from(3));
    TS_ASSERT_SAME_DATA(ca->raw_bytes(), "yzz", 3);
  }

  void test_fetch_bytes_out_of_bounds() {
    CharArray* c = String::create(state, "xyzzy")->data();

    Fixnum* neg = Fixnum::from(-1);
    Fixnum* zero = Fixnum::from(0);
    Fixnum* one = Fixnum::from(1);
    Fixnum* size = c->size(state);

    TS_ASSERT_THROWS_ASSERT(c->fetch_bytes(state, neg, zero), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(c->fetch_bytes(state, zero, neg), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));

    TS_ASSERT_THROWS_ASSERT(c->fetch_bytes(state, one, size), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_compare_bytes() {
    CharArray* a = String::create(state, "xyZzyx")->data();
    CharArray* b = String::create(state, "xyzzyx")->data();

    Fixnum* two = Fixnum::from(2);
    Fixnum* three = Fixnum::from(3);
    Fixnum* size = Fixnum::from(8);
    Fixnum* size1 = Fixnum::from(9);

    TS_ASSERT_EQUALS(a->size(state)->to_native(), 8);
    TS_ASSERT_EQUALS(b->size(state)->to_native(), 8);

    TS_ASSERT_EQUALS(a->compare_bytes(state, b, two, two), Fixnum::from(0));
    TS_ASSERT_EQUALS(a->compare_bytes(state, b, two, three), Fixnum::from(-1));
    TS_ASSERT_EQUALS(a->compare_bytes(state, b, three, two), Fixnum::from(1));
    TS_ASSERT_EQUALS(a->compare_bytes(state, b, three, three), Fixnum::from(-1));
    TS_ASSERT_EQUALS(a->compare_bytes(state, b, size, size), Fixnum::from(-1));
    TS_ASSERT_EQUALS(a->compare_bytes(state, b, size1, size1), Fixnum::from(-1));
  }

  void test_compare_bytes_out_of_bounds() {
    CharArray* a = String::create(state, "xyZzy")->data();
    CharArray* b = String::create(state, "xyzzy")->data();

    Fixnum* zero = Fixnum::from(0);
    Fixnum* neg = Fixnum::from(-1);

    TS_ASSERT_THROWS_ASSERT(a->compare_bytes(state, b, neg, zero), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(a->compare_bytes(state, b, zero, neg), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_locate() {
    CharArray* c = String::create(state, "xyZfoo\nzyx")->data();
    Fixnum* zero = Fixnum::from(0);
    Fixnum* three = Fixnum::from(3);
    Fixnum* seven = Fixnum::from(7);
    Fixnum* four = Fixnum::from(4);
    Fixnum* two = Fixnum::from(2);

    String* foo_nl = String::create(state, "foo\n");

    TS_ASSERT_EQUALS(three, c->locate(state, String::create(state, ""), three, zero));
    TS_ASSERT_EQUALS(Qnil, c->locate(state, String::create(state, "\n\n"), zero, zero));
    TS_ASSERT_EQUALS(seven, (Fixnum*)c->locate(state, String::create(state, "\n"), zero, zero));
    TS_ASSERT_EQUALS(Qnil, c->locate(state, foo_nl, four, zero));
    TS_ASSERT_EQUALS(seven, (Fixnum*)c->locate(state, foo_nl, two, zero));
    TS_ASSERT_EQUALS(seven, (Fixnum*)c->locate(state, foo_nl, three, zero));
    TS_ASSERT_EQUALS(Fixnum::from(10), (Fixnum*)c->locate(state,
                     String::create(state, "yx"), three, zero));
  }
};
