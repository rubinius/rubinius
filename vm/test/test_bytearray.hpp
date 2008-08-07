#include "objects.hpp"
#include "vm.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/string.hpp"

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
    TS_ASSERT_EQUALS(b->size(state)->n2i(), 4);

    b = ByteArray::create(state, 54);
    TS_ASSERT_EQUALS(b->size(state)->n2i(), 56);

    b = ByteArray::create(state, 55);
    TS_ASSERT_EQUALS(b->size(state)->n2i(), 60);

    b = ByteArray::create(state, 58);
    TS_ASSERT_EQUALS(b->size(state)->n2i(), 60);

    b = ByteArray::create(state, 59);
    TS_ASSERT_EQUALS(b->size(state)->n2i(), 64);
  }

  void test_to_chars() {
    String* s = String::create(state, "xy");
    ByteArray* b = s->data;
    char* chars = b->to_chars(state);

    TS_ASSERT_SAME_DATA("xy", chars, 2);
  }

};

