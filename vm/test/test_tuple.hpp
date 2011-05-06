#include "vm/test/test.hpp"

#include "builtin/tuple.hpp"

class TestTuple : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
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
    TS_ASSERT_EQUALS(2, tuple->num_fields());
  }

  void test_at() {
    Tuple* tuple = new_tuple();
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(tuple->at(state, 1)));
    TS_ASSERT_EQUALS(Qnil, tuple->at(state, 4));
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

  void test_copy_from() {
    Tuple* tuple = new_tuple();

    Tuple* dest = Tuple::create(state, 3);
    dest->copy_from(state, tuple, Fixnum::from(0), Fixnum::from(3), Fixnum::from(0));

    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 1)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(state, 2)));

    dest->copy_from(state, tuple, Fixnum::from(1), Fixnum::from(2), Fixnum::from(0));

    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(state, 1)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(state, 2)));

    dest->copy_from(state, tuple, Fixnum::from(0), Fixnum::from(2), Fixnum::from(1));

    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(state, 1)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 2)));
  }

  void test_copy_from_start_out_of_range() {
    Tuple* tuple = new_tuple();
    Tuple* dest = Tuple::create(state, 2);
    TS_ASSERT_THROWS_ASSERT(dest->copy_from(state, tuple, Fixnum::from(3), Fixnum::from(1),
					    Fixnum::from(0)),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(dest->copy_from(state, tuple, Fixnum::from(-1), Fixnum::from(1),
					    Fixnum::from(0)),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_copy_from_dest_out_of_range() {
    Tuple* tuple = new_tuple();
    Tuple* dest = Tuple::create(state, 2);
    TS_ASSERT_THROWS_ASSERT(dest->copy_from(state, tuple, Fixnum::from(0), Fixnum::from(1),
					    Fixnum::from(2)),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(dest->copy_from(state, tuple, Fixnum::from(0), Fixnum::from(1),
					    Fixnum::from(-1)),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_copy_from_to_empty_this() {
    Tuple* tuple = new_tuple();
    Tuple* dest = Tuple::create(state, 0);
    dest->copy_from(state, tuple, Fixnum::from(0), Fixnum::from(0), Fixnum::from(0));
  }

  void test_copy_from_other_empty() {
    Tuple* tuple = Tuple::create(state, 0);
    Tuple* dest = new_tuple();
    dest->copy_from(state, tuple, Fixnum::from(0), Fixnum::from(0), Fixnum::from(0));
    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(dest->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(4), as<Fixnum>(dest->at(state, 1)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(dest->at(state, 2)));
  }

  void test_copy_from_length_positive() {
    Tuple* tuple = new_tuple();
    Tuple* dest = Tuple::create(state, 2);
    TS_ASSERT_THROWS_ASSERT(dest->copy_from(state, tuple, Fixnum::from(0), Fixnum::from(-1),
					    Fixnum::from(0)),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_copy_from_length_exceeds_source() {
    Tuple* tuple = new_tuple();
    Tuple* dest = Tuple::create(state, 3);
    TS_ASSERT_THROWS_ASSERT(dest->copy_from(state, tuple, Fixnum::from(1), Fixnum::from(3),
					    Fixnum::from(0)),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_copy_from_length_exceeds_destination() {
    Tuple* tuple = new_tuple();
    Tuple* dest = Tuple::create(state, 2);
    TS_ASSERT_THROWS_ASSERT(dest->copy_from(state, tuple, Fixnum::from(0), Fixnum::from(3),
					    Fixnum::from(0)),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_delete_inplace() {
    Tuple *tuple = new_tuple();

    tuple->put(state, 1, Qnil);
    Integer *count = tuple->delete_inplace(state, Fixnum::from(0), Fixnum::from(3), Qnil);

    TS_ASSERT_EQUALS(1, count->to_native());
    TS_ASSERT_EQUALS(Fixnum::from(1), as<Fixnum>(tuple->at(state, 0)));
    TS_ASSERT_EQUALS(Fixnum::from(9), as<Fixnum>(tuple->at(state, 1)));
    TS_ASSERT_EQUALS(Qnil, tuple->at(state, 2));
  }

  void test_delete_inplace_bounds() {
    Tuple *tuple = new_tuple();

    TS_ASSERT_THROWS_ASSERT(tuple->delete_inplace(state, Fixnum::from(0), Fixnum::from(4), Qnil),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));

    TS_ASSERT_THROWS_ASSERT(tuple->delete_inplace(state, Fixnum::from(1), Fixnum::from(3), Qnil),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));

    TS_ASSERT_THROWS_ASSERT(tuple->delete_inplace(state, Fixnum::from(-1), Fixnum::from(3), Qnil),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));

    TS_ASSERT_THROWS_ASSERT(tuple->delete_inplace(state, Fixnum::from(0), Fixnum::from(-1), Qnil),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));

    TS_ASSERT_THROWS_ASSERT(tuple->delete_inplace(state, Fixnum::from(3), Fixnum::from(1), Qnil),
			    const RubyException &e,
			    TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_pattern() {
    Fixnum* ten = Fixnum::from(10);
    Tuple* tuple = Tuple::pattern(state, Fixnum::from(5), ten);

    TS_ASSERT_EQUALS(5, tuple->num_fields());
    for(size_t i = 0; i < 5; i++) {
      TS_ASSERT_EQUALS(ten, tuple->at(state, i));
    }
  }
};
