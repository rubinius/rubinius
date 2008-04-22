#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestFixnum : public CxxTest::TestSuite {
  public:

#undef state
  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_add() {
    FIXNUM one = as<Fixnum>(Object::i2n(1));

    FIXNUM two = as<Fixnum>(one->add(state, one));
    TS_ASSERT_EQUALS(two->n2i(), 2);
  }

  void test_sub() {
    FIXNUM one = as<Fixnum>(Object::i2n(1));

    FIXNUM zero = as<Fixnum>(one->sub(state, one));
    TS_ASSERT_EQUALS(zero->n2i(), 0);
  }

  void test_multiply() {
    FIXNUM one = as<Fixnum>(Object::i2n(4));

    FIXNUM two = as<Fixnum>(one->multiply(state, one));
    TS_ASSERT_EQUALS(two->n2i(), 16);
  }

  void test_divide() {
    FIXNUM one = as<Fixnum>(Object::i2n(4));

    FIXNUM two = as<Fixnum>(one->divide(state, one));
    TS_ASSERT_EQUALS(two->n2i(), 1);
  }

  void test_modulo() {
    FIXNUM one = as<Fixnum>(Object::i2n(4));

    FIXNUM two = as<Fixnum>(one->modulo(state, one));
    TS_ASSERT_EQUALS(two->n2i(), 0);
  }
};
