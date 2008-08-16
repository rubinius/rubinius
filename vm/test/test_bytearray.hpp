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

  void test_size() {
    ByteArray* b;

    b = ByteArray::create(state, 0);
    TS_ASSERT_EQUALS(b->size(state)->to_native(), 4);

    b = ByteArray::create(state, 54);
    TS_ASSERT_EQUALS(b->size(state)->to_native(), 56);

    b = ByteArray::create(state, 55);
    TS_ASSERT_EQUALS(b->size(state)->to_native(), 60);

    b = ByteArray::create(state, 58);
    TS_ASSERT_EQUALS(b->size(state)->to_native(), 60);

    b = ByteArray::create(state, 59);
    TS_ASSERT_EQUALS(b->size(state)->to_native(), 64);
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
};
