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

  void test_tuple_fields() {
    TS_ASSERT_EQUALS(0U, Tuple::fields);
  }

  void test_allocate() {
    Tuple* tuple = Tuple::allocate(state, Fixnum::from(2));
    TS_ASSERT_EQUALS(2U, tuple->num_fields());
  }

  void test_at() {
    Tuple* tuple = new_tuple();
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(tuple->at(state, 1)));
    TS_ASSERT_THROWS_ASSERT(tuple->at(state, 4), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_at_prim() {
    Tuple* tuple = new_tuple();
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(tuple->at_prim(state, Fixnum::from(1))));
    TS_ASSERT_THROWS_ASSERT(tuple->at_prim(state, Fixnum::from(4)), const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
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

  void test_copy_range() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->copy_range(state, tuple, 1, 2, 0);

    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(state, 1)));
  }

  void test_copy_range_this_shorter() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->put(state, 0, Fixnum::from(42));
    dest->put(state, 1, Fixnum::from(42));

    dest->copy_range(state, tuple, 2, 3, 0);

    TS_ASSERT_EQUALS(Fixnum::from(9),  as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(42), as<Fixnum>(dest->at(state, 1)));
  }

  void test_copy_range_other_shorter() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->copy_range(state, tuple, 0, 2, 0);

    TS_ASSERT_EQUALS(Fixnum::from(1),  as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 1)));
  }

  void test_copy_range_start_not_less_than_zero() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->copy_range(state, tuple, -2, 2, 0);

    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 1)));
  }

  void test_copy_range_with_dest() {
    Tuple *tuple = new_tuple();
    Tuple *dest = Tuple::create(state, 3);
    dest->copy_range(state, tuple, 0, 1, 1);

    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(state, 1)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 2)));    
  }

  void test_copy_range_wtih_source_empty() {
    Tuple *tuple = Tuple::create(state, 0);
    Tuple *dest = new_tuple();

    dest->copy_range(state, tuple, 0, 1, 0);

    TS_ASSERT_EQUALS(0U, tuple->num_fields());
    TS_ASSERT_EQUALS(3U, dest->num_fields());
    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 1)));    
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(state, 2)));    
  }

  void test_copy_from() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->copy_from(state, tuple, Fixnum::from(1), Fixnum::from(0));

    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(state, 1)));

    dest->copy_from(state, tuple, Fixnum::from(1), Fixnum::from(1));

    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 1)));
  }

  void test_copy_from_other_shorter() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->put(state, 0, Fixnum::from(42));
    dest->put(state, 1, Fixnum::from(42));
    dest->copy_from(state, tuple, Fixnum::from(2), Fixnum::from(0));

    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(42), as<Fixnum>(dest->at(state, 1)));
  }

  void test_copy_from_this_shorter() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->copy_from(state, tuple, Fixnum::from(0), Fixnum::from(0));

    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 1)));
  }

  void test_copy_from_start_not_less_than_zero() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 2);
    dest->copy_from(state, tuple, Fixnum::from(-1), Fixnum::from(0));

    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 1)));

    dest->copy_from(state, tuple, Fixnum::from(1), Fixnum::from(-1));

    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(state, 1)));
  }

  void test_pattern() {
    Fixnum* ten = Fixnum::from(10);
    Tuple* tuple = Tuple::pattern(state, Fixnum::from(5), ten);

    TS_ASSERT_EQUALS(5U, tuple->num_fields());
    for(size_t i = 0; i < 5; i++) {
      TS_ASSERT_EQUALS(ten, tuple->at(state, i));
    }
  }
};
