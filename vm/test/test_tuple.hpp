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

  Tuple* new_tuple() {
    Tuple* tuple = Tuple::create(state, 3);
    tuple->put(state, 0, Fixnum::from(1));
    tuple->put(state, 1, Fixnum::from(4));
    tuple->put(state, 2, Fixnum::from(9));
    return tuple;
  }

  void test_allocate() {
    Tuple* tuple = Tuple::allocate(state, Fixnum::from(2));
    TS_ASSERT_EQUALS(2U, tuple->num_fields());
  }

  void test_at() {
    Tuple* tuple = new_tuple();
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(tuple->at(1)));
    TS_ASSERT_THROWS(tuple->at(4), const ObjectBoundsExceeded &);
  }

  void test_at_prim() {
    Tuple* tuple = new_tuple();
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(tuple->at_prim(state, Fixnum::from(1))));
    TS_ASSERT_EQUALS(Qnil, tuple->at_prim(state, Fixnum::from(4)));
  }

  void test_put_prim() {
    Tuple* tuple = new_tuple();
    tuple->put_prim(state, Fixnum::from(1), Fixnum::from(22));
    TS_ASSERT_EQUALS(Fixnum::from(22), as<Fixnum>(tuple->at_prim(state, Fixnum::from(1))));
  }

  void test_fields_prim() {
    Tuple* tuple = new_tuple();
    TS_ASSERT_EQUALS(Fixnum::from(3), as<Fixnum>(tuple->fields_prim(state)));
  }

  void test_replace_with() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->replace_with(state, tuple, 1, 2);

    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(1)));
  }

  void test_replace_with_this_shorter() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->put(state, 0, Fixnum::from(42));
    dest->put(state, 1, Fixnum::from(42));

    dest->replace_with(state, tuple, 2, 3);

    TS_ASSERT_EQUALS(Fixnum::from(9),  as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(42), as<Fixnum>(dest->at(1)));
  }

  void test_replace_with_other_shorter() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->replace_with(state, tuple, 0, 2);

    TS_ASSERT_EQUALS(Fixnum::from(1),  as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(1)));
  }

  void test_replace_with_start_not_less_than_zero() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->replace_with(state, tuple, -2, 2);

    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(1)));
  }

  void test_copy_from() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->copy_from(state, tuple, Fixnum::from(1), Fixnum::from(0));

    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(1)));

    dest->copy_from(state, tuple, Fixnum::from(1), Fixnum::from(1));

    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(1)));
  }

  void test_copy_from_other_shorter() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->put(state, 0, Fixnum::from(42));
    dest->put(state, 1, Fixnum::from(42));
    dest->copy_from(state, tuple, Fixnum::from(2), Fixnum::from(0));

    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(42), as<Fixnum>(dest->at(1)));
  }

  void test_copy_from_this_shorter() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->copy_from(state, tuple, Fixnum::from(0), Fixnum::from(0));

    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(1)));
  }

  void test_copy_from_start_not_less_than_zero() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->copy_from(state, tuple, Fixnum::from(-1), Fixnum::from(0));

    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(1)));

    dest->copy_from(state, tuple, Fixnum::from(1), Fixnum::from(-1));

    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(0)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(1)));
  }

  void test_shifted_by_zero() {
    Tuple* tuple = new_tuple();

    Tuple* dest = tuple->shifted(state, Fixnum::from(0));
    TS_ASSERT_EQUALS(tuple, dest);
  }

  void test_shifted_by_amount() {
    Tuple* tuple = new_tuple();

    Tuple* dest = tuple->shifted(state, Fixnum::from(4));

    TS_ASSERT_EQUALS(dest->num_fields(), 7U);
    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(4)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(5)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(6)));
  }

  void test_pattern() {
    FIXNUM ten = Fixnum::from(10);
    Tuple* tuple = Tuple::pattern(state, Fixnum::from(5), ten);

    TS_ASSERT_EQUALS(5U, tuple->num_fields());
    for(size_t i = 0; i < 5; i++) {
      TS_ASSERT_EQUALS(ten, tuple->at(i));
    }
  }
};
