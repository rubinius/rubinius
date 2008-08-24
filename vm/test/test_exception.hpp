#include "object_types.hpp"
#include "exception.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestException : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_argument_error_raise() {
    TS_ASSERT_THROWS(ArgumentError::raise(2, 1), const ArgumentError &);
  }

  void test_type_error_raise() {
    object_type type = ClassType;
    OBJECT obj = G(object);

    TS_ASSERT_THROWS(TypeError::raise(type, obj, "failed"), const TypeError &);
  }

  void test_type_error_raise_without_reason() {
    object_type type = ClassType;
    OBJECT obj = G(object);

    TS_ASSERT_THROWS(TypeError::raise(type, obj), const TypeError &);
  }

  void test_assertion_raise() {
    TS_ASSERT_THROWS(Assertion::raise("failed"), const Assertion &);
  }

  void test_object_bounds_exceeded_raise() {
    TS_ASSERT_THROWS(ObjectBoundsExceeded::raise(G(object), 1U), const ObjectBoundsExceeded &);
  }
};
