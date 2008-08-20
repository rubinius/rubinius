#include "builtin/tuple.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestTuple : public CxxTest::TestSuite {
public:

  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_copy_from() {
    Tuple* tuple = Tuple::create(state, 3);
    tuple->put(state, 0, Fixnum::from(1));
    tuple->put(state, 1, Fixnum::from(4));
    tuple->put(state, 2, Fixnum::from(9));

    Tuple* dest = Tuple::create(state, 2);
    dest->copy_from(state, tuple, 1, 2);

    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(1)));
  }

  void test_copy_from_shorter() {
    Tuple* tuple = Tuple::create(state, 3);
    tuple->put(state, 0, Fixnum::from(1));
    tuple->put(state, 1, Fixnum::from(4));
    tuple->put(state, 2, Fixnum::from(9));

    Tuple* dest = Tuple::create(state, 2);
    dest->put(state, 0, Fixnum::from(42));
    dest->put(state, 1, Fixnum::from(42));

    dest->copy_from(state, tuple, 2, 3);

    TS_ASSERT_EQUALS(Fixnum::from(9),  as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(42), as<Fixnum>(dest->at(1)));
  }

};

