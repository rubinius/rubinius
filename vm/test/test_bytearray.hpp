#include "vm/test/test.hpp"

#include "vm/object_utils.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/exception.hpp"
#include "builtin/string.hpp"
#include "primitives.hpp"

class TestByteArray : public CxxTest::TestSuite, public VMTest {
  public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  ByteArray* new_bytearray(const char* str, int size = 0) {
    if(size == 0) size = strlen(str);
    ByteArray* b = ByteArray::create(state, size);
    memcpy(b->raw_bytes(), str, size);
    return b;
  }

  void test_create() {
    size_t mag = sizeof(Object*);
    ByteArray* b;

    b = ByteArray::create(state, 0);
    TS_ASSERT_EQUALS(b->size(state)->to_native(), (native_int)0);

    for(size_t i = 1; i <= mag; i++) {
      b = ByteArray::create(state, i);
      TS_ASSERT_EQUALS(b->size(state)->to_native(), (native_int)mag);
    }

    for(size_t i = mag + 1; i <= 2 * mag; i++) {
      b = ByteArray::create(state, i);
      TS_ASSERT_EQUALS(b->size(state)->to_native(), (native_int)(2 * mag));
    }

    for(size_t i = 5 * mag + 1; i <= 6 * mag; i++) {
      b = ByteArray::create(state, i);
      TS_ASSERT_EQUALS(b->size(state)->to_native(), (native_int)(6 * mag));
    }
  }

  void test_allocate() {
    ByteArray* b = ByteArray::allocate(state, Fixnum::from(5));
    TS_ASSERT_EQUALS(b->size(state)->to_native(), 8);
  }

  void test_size() {
    ByteArray* b = ByteArray::create(state, 1);
    TS_ASSERT_EQUALS(b->size(state), Fixnum::from(sizeof(Object*)));
  }

  void test_get_byte() {
    ByteArray* b = new_bytearray("xyz");
    TS_ASSERT_EQUALS(b->get_byte(state, Fixnum::from(0)), Fixnum::from('x'));
    TS_ASSERT_EQUALS(b->get_byte(state, Fixnum::from(2)), Fixnum::from('z'));
  }

  void test_get_byte_index_out_of_bounds() {
    ByteArray* b = new_bytearray("xyz");
    native_int sz = b->size(state)->to_native();
    TS_ASSERT_THROWS_ASSERT(b->get_byte(state, Fixnum::from(sz)), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(b->get_byte(state, Fixnum::from(sz+1)), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(b->get_byte(state, Fixnum::from(-1)), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_set_byte() {
    ByteArray* b = new_bytearray("xyz");
    b->set_byte(state, Fixnum::from(0), Fixnum::from('1'));
    TS_ASSERT_EQUALS(b->get_byte(state, Fixnum::from(0)), Fixnum::from('1'));
    b->set_byte(state, Fixnum::from(2), Fixnum::from('2'));
    TS_ASSERT_EQUALS(b->get_byte(state, Fixnum::from(2)), Fixnum::from('2'));
  }

  void test_set_byte_out_of_bounds() {
    ByteArray* b = new_bytearray("xyz");
    native_int sz = b->size(state)->to_native();
    TS_ASSERT_THROWS_ASSERT(b->set_byte(state, Fixnum::from(sz), Fixnum::from('0')),
        const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(b->set_byte(state, Fixnum::from(sz+1), Fixnum::from('0')),
        const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(b->set_byte(state, Fixnum::from(-1), Fixnum::from('0')),
        const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_move_bytes() {
    ByteArray* b = new_bytearray("xyzzy");
    b->move_bytes(state, Fixnum::from(0), Fixnum::from(2), Fixnum::from(3));
    TS_ASSERT_SAME_DATA(b->raw_bytes(), "xyzxy", 5);
  }

  void test_move_bytes_out_of_bounds() {
    ByteArray* b = new_bytearray("xyzzy");

    Fixnum* neg = Fixnum::from(-1);
    Fixnum* one = Fixnum::from(1);
    Fixnum* zero = Fixnum::from(0);
    Fixnum* size = b->size(state);

    TS_ASSERT_THROWS_ASSERT(b->move_bytes(state, neg, zero, zero), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(b->move_bytes(state, zero, neg, zero), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(b->move_bytes(state, zero, zero, neg), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));

    TS_ASSERT_THROWS_ASSERT(b->move_bytes(state, zero, size, one), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(b->move_bytes(state, one, size, zero), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_fetch_bytes() {
    ByteArray* b = new_bytearray("xyzzy");
    ByteArray* ba = b->fetch_bytes(state, Fixnum::from(1), Fixnum::from(3));
    TS_ASSERT_SAME_DATA(ba->raw_bytes(), "yzz", 3);
  }

  void test_fetch_bytes_out_of_bounds() {
    ByteArray* b = new_bytearray("xyzzy");

    Fixnum* neg = Fixnum::from(-1);
    Fixnum* zero = Fixnum::from(0);
    Fixnum* one = Fixnum::from(1);
    Fixnum* size = b->size(state);

    TS_ASSERT_THROWS_ASSERT(b->fetch_bytes(state, neg, zero), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(b->fetch_bytes(state, zero, neg), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));

    TS_ASSERT_THROWS_ASSERT(b->fetch_bytes(state, one, size), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_compare_bytes() {
    ByteArray* a = new_bytearray("xyZzyx");
    ByteArray* b = new_bytearray("xyzzyx");

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
    ByteArray* a = new_bytearray("xyZzy");
    ByteArray* b = new_bytearray("xyzzy");

    Fixnum* zero = Fixnum::from(0);
    Fixnum* neg = Fixnum::from(-1);

    TS_ASSERT_THROWS_ASSERT(a->compare_bytes(state, b, neg, zero), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(a->compare_bytes(state, b, zero, neg), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_locate() {
    ByteArray* ba = String::create(state, "xyZfoo\nzyx")->data();
    Fixnum* size = Fixnum::from(ba->size());
    Fixnum* zero = Fixnum::from(0);
    Fixnum* three = Fixnum::from(3);
    Fixnum* seven = Fixnum::from(7);
    Fixnum* four = Fixnum::from(4);
    Fixnum* two = Fixnum::from(2);

    String* foo_nl = String::create(state, "foo\n");

    TS_ASSERT_EQUALS(three, ba->locate(state, String::create(state, ""), three, size));
    TS_ASSERT_EQUALS(cNil, ba->locate(state, String::create(state, "\n\n"), zero, size));
    TS_ASSERT_EQUALS(seven, ba->locate(state, String::create(state, "\n"), zero, size));
    TS_ASSERT_EQUALS(cNil, ba->locate(state, foo_nl, four, size));
    TS_ASSERT_EQUALS(seven, ba->locate(state, foo_nl, two, size));
    TS_ASSERT_EQUALS(seven, ba->locate(state, foo_nl, three, size));
    TS_ASSERT_EQUALS(Fixnum::from(10), ba->locate(state,
                     String::create(state, "yx"), three, size));
  }
};
