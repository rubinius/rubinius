#include "objects.hpp"
#include "vm.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/string.hpp"
#include "primitives.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestByteArray : public CxxTest::TestSuite {
  public:

  VM * state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    size_t mag = sizeof(OBJECT);
    ByteArray* b;

    for(size_t i = 0; i <= mag; i++) {
      b = ByteArray::create(state, i);
      TS_ASSERT_EQUALS(b->size(state)->to_native(), mag);
    }

    for(size_t i = mag + 1; i <= 2 * mag; i++) {
      b = ByteArray::create(state, i);
      TS_ASSERT_EQUALS(b->size(state)->to_native(), 2 * mag);
    }

    for(size_t i = 5 * mag + 1; i <= 6 * mag; i++) {
      b = ByteArray::create(state, i);
      TS_ASSERT_EQUALS(b->size(state)->to_native(), 6 * mag);
    }
  }

  void test_allocate() {
    ByteArray* b = ByteArray::allocate(state, Fixnum::from(5));
    TS_ASSERT_EQUALS(b->size(state)->to_native(), 8);
  }

  void test_size() {
    ByteArray* b = ByteArray::create(state, 0);
    TS_ASSERT_EQUALS(b->size(state), Fixnum::from(4));
  }

  void test_to_chars() {
    String* s = String::create(state, "xy");
    ByteArray* b = s->data;
    char* chars = b->to_chars(state);

    TS_ASSERT_SAME_DATA("xy", chars, 2);
  }

  void test_get_byte() {
    ByteArray* b = String::create(state, "xyz")->data;
    TS_ASSERT_EQUALS(b->get_byte(state, Fixnum::from(0)), Fixnum::from('x'));
    TS_ASSERT_EQUALS(b->get_byte(state, Fixnum::from(2)), Fixnum::from('z'));
  }

  void test_get_byte_index_out_of_bounds() {
    ByteArray* b = String::create(state, "xyz")->data;
    native_int sz = b->size(state)->to_native();
    TS_ASSERT_THROWS(b->get_byte(state, Fixnum::from(sz)), const PrimitiveFailed &);
    TS_ASSERT_THROWS(b->get_byte(state, Fixnum::from(sz+1)), const PrimitiveFailed &);
    TS_ASSERT_THROWS(b->get_byte(state, Fixnum::from(-1)), const PrimitiveFailed &);
  }

  void test_set_byte() {
    ByteArray* b = String::create(state, "xyz")->data;
    b->set_byte(state, Fixnum::from(0), Fixnum::from('1'));
    TS_ASSERT_EQUALS(b->get_byte(state, Fixnum::from(0)), Fixnum::from('1'));
    b->set_byte(state, Fixnum::from(2), Fixnum::from('2'));
    TS_ASSERT_EQUALS(b->get_byte(state, Fixnum::from(2)), Fixnum::from('2'));
  }

  void test_set_byte_out_of_bounds() {
    ByteArray* b = String::create(state, "xyz")->data;
    native_int sz = b->size(state)->to_native();
    TS_ASSERT_THROWS(b->set_byte(state, Fixnum::from(sz), Fixnum::from('0')), const PrimitiveFailed &);
    TS_ASSERT_THROWS(b->set_byte(state, Fixnum::from(sz+1), Fixnum::from('0')), const PrimitiveFailed &);
    TS_ASSERT_THROWS(b->set_byte(state, Fixnum::from(-1), Fixnum::from('0')), const PrimitiveFailed &);
  }

  void test_move_bytes() {
    String* s = String::create(state, "xyzzy");
    ByteArray* b = s->data;
    b->move_bytes(state, Fixnum::from(0), Fixnum::from(2), Fixnum::from(3));
    TS_ASSERT_SAME_DATA(b->bytes, "xyzxy", 5);
  }

  void test_move_bytes_out_of_bounds() {
    ByteArray* b = String::create(state, "xyzzy")->data;

    INTEGER neg = Fixnum::from(-1);
    INTEGER one = Fixnum::from(1);
    INTEGER zero = Fixnum::from(0);
    INTEGER size = b->size(state);
    INTEGER size1 = Fixnum::from(b->size(state)->to_native()+1);

    TS_ASSERT_THROWS(b->move_bytes(state, neg, zero, zero), const PrimitiveFailed &);
    TS_ASSERT_THROWS(b->move_bytes(state, zero, neg, zero), const PrimitiveFailed &);
    TS_ASSERT_THROWS(b->move_bytes(state, zero, zero, neg), const PrimitiveFailed &);

    TS_ASSERT_THROWS(b->move_bytes(state, zero, size1, zero), const PrimitiveFailed &);
    TS_ASSERT_THROWS(b->move_bytes(state, zero, size, one), const PrimitiveFailed &);
    TS_ASSERT_THROWS(b->move_bytes(state, one, size, zero), const PrimitiveFailed &);
  }

  void test_fetch_bytes() {
    String* s = String::create(state, "xyzzy");
    ByteArray* b = s->data;
    ByteArray* ba = b->fetch_bytes(state, Fixnum::from(1), Fixnum::from(3));
    TS_ASSERT_SAME_DATA(ba->bytes, "yzz", 3);
  }

  void test_fetch_bytes_out_of_bounds() {
    ByteArray* b = String::create(state, "xyzzy")->data;

    INTEGER neg = Fixnum::from(-1);
    INTEGER zero = Fixnum::from(0);
    INTEGER one = Fixnum::from(1);
    INTEGER size = b->size(state);
    INTEGER size1 = Fixnum::from(b->size(state)->to_native()+1);

    TS_ASSERT_THROWS(b->fetch_bytes(state, neg, zero), const PrimitiveFailed &);
    TS_ASSERT_THROWS(b->fetch_bytes(state, zero, neg), const PrimitiveFailed &);

    TS_ASSERT_THROWS(b->fetch_bytes(state, zero, size1), const PrimitiveFailed &);
    TS_ASSERT_THROWS(b->fetch_bytes(state, one, size), const PrimitiveFailed &);
  }

  void test_compare_bytes() {
    ByteArray* a = String::create(state, "xyZzyx")->data;
    ByteArray* b = String::create(state, "xyzzyx")->data;

    INTEGER two = Fixnum::from(2);
    INTEGER three = Fixnum::from(3);
    INTEGER size = Fixnum::from(8);
    INTEGER size1 = Fixnum::from(9);

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
    ByteArray* a = String::create(state, "xyZzy")->data;
    ByteArray* b = String::create(state, "xyzzy")->data;

    INTEGER zero = Fixnum::from(0);
    INTEGER neg = Fixnum::from(-1);

    TS_ASSERT_THROWS(a->compare_bytes(state, b, neg, zero), const PrimitiveFailed &);
    TS_ASSERT_THROWS(a->compare_bytes(state, b, zero, neg), const PrimitiveFailed &);
  }

  void test_dup_into() {
    ByteArray* a = String::create(state, "xyZzyx")->data;
    ByteArray* b = ByteArray::create(state, 5);

    TS_ASSERT_EQUALS(a->size(state)->to_native(), 8);
    TS_ASSERT_EQUALS(b->size(state)->to_native(), 8);

    a->dup_into(state, b);
    TS_ASSERT_SAME_DATA(b->bytes, "xyZzyx", 7);

    ByteArray* c = ByteArray::create(state, 0);
    TS_ASSERT_EQUALS(c->size(state)->to_native(), 4);
    a->dup_into(state, c);
    TS_ASSERT_SAME_DATA(c->bytes, "xyZz", 4);

    ByteArray* d = ByteArray::create(state, 12);
    a->dup_into(state, d);
    TS_ASSERT_SAME_DATA(d->bytes, "xyZzyx", 7);
  }
};
