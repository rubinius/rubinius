#include "objects.hpp"
#include "vm.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestString : public CxxTest::TestSuite {
  public:

#undef state
  VM *state;
  String *str;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    str = String::create(state, "blah");
    TS_ASSERT_EQUALS(str->bytes->n2i(), 4);
  }

  void test_create2() {
    str = String::create(state, "blah", 2);
    TS_ASSERT_EQUALS(str->bytes->n2i(), 2);
  }

  void test_hash_string() {
    str = String::create(state, "blah");
    hashval hash = str->hash_string(state);
    TS_ASSERT(hash > 0);

    hashval again = str->hash_string(state);

    TS_ASSERT_EQUALS(hash, again);
  }
};
