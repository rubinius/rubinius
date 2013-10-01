#include "vm/test/test.hpp"

#include "builtin/tuple.hpp"
#include "builtin/list.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/array.hpp"
#include "builtin/iseq.hpp"
#include "builtin/compiled_code.hpp"
#include "builtin/symbol.hpp"

#include "arguments.hpp"

class TestArguments : public CxxTest::TestSuite, public VMTest {
public:
  Object** static_args;
  Fixnum* three;
  Fixnum* four;

  void setUp() {
    create();

    three = Fixnum::from(3);
    four = Fixnum::from(4);

    static_args = new Object*[2];
    static_args[0] = three;
    static_args[1] = four;
  }

  void tearDown() {
    delete[] static_args;
    destroy();
  }

  void test_prepend() {
    Arguments args(state->symbol("blah"), 2, static_args);

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, state->symbol("blah"));
    ary->set(state, 1, state->symbol("foo"));
    args.prepend(state, ary);

    TS_ASSERT_EQUALS(state->symbol("blah"), args.get_argument(0));
    TS_ASSERT_EQUALS(state->symbol("foo"),  args.get_argument(1));
    TS_ASSERT_EQUALS(Fixnum::from(3),       args.get_argument(2));
    TS_ASSERT_EQUALS(Fixnum::from(4),       args.get_argument(3));

    TS_ASSERT_EQUALS(4U, args.total());
  }

  void test_append() {
    Arguments args(state->symbol("blah"), 2, static_args);

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, state->symbol("blah"));
    ary->set(state, 1, state->symbol("foo"));
    args.append(state, ary);

    TS_ASSERT_EQUALS(Fixnum::from(3),       args.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(4),       args.get_argument(1));
    TS_ASSERT_EQUALS(state->symbol("blah"), args.get_argument(2));
    TS_ASSERT_EQUALS(state->symbol("foo"),  args.get_argument(3));

    TS_ASSERT_EQUALS(4U, args.total());
  }

  void test_get_argument_from_array() {
    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Fixnum::from(3));
    ary->set(state, 1, Fixnum::from(4));

    Arguments args(state->symbol("blah"), ary);

    TS_ASSERT_EQUALS(Fixnum::from(3), args.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(4), args.get_argument(1));
    TS_ASSERT_EQUALS(2U, args.total());
  }

  void test_unshift() {
    Arguments args(state->symbol("blah"), 1, static_args);

    TS_ASSERT_EQUALS(1U, args.total());
    args.unshift(state, Fixnum::from(47));
    TS_ASSERT_EQUALS(2U, args.total());

    TS_ASSERT_EQUALS(Fixnum::from(47), args.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(3), args.get_argument(1));
  }

  void test_unshift_into_splat() {
    Arguments args(state->symbol("blah"), 2, static_args);
    Array* ary = Array::create(state, 1);
    ary->set(state, 0, Fixnum::from(3));

    args.use_array(ary);

    TS_ASSERT_EQUALS(1U, args.total());
    args.unshift(state, Fixnum::from(47));
    TS_ASSERT_EQUALS(2U, args.total());

    TS_ASSERT_EQUALS(Fixnum::from(47), args.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(3), args.get_argument(1));
  }

  void test_unshit_argument_multiple_times() {
    Arguments args(state->symbol("blah"), 1, static_args);

    TS_ASSERT_EQUALS(1U, args.total());
    args.unshift(state, Fixnum::from(47));
    args.unshift(state, Fixnum::from(8));
    TS_ASSERT_EQUALS(3U, args.total());

    TS_ASSERT_EQUALS(Fixnum::from(8), args.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(47), args.get_argument(1));
    TS_ASSERT_EQUALS(Fixnum::from(3), args.get_argument(2));

  }

  void test_shift() {
    Arguments args(state->symbol("blah"), 2, static_args);

    TS_ASSERT_EQUALS(2U, args.total());

    Fixnum* shifted = as<Fixnum>(args.shift(state));
    TS_ASSERT_EQUALS(shifted, Fixnum::from(3));
    TS_ASSERT_EQUALS(args.total(), 1U);

    TS_ASSERT_EQUALS(args.get_argument(0), Fixnum::from(4));
  }

  void test_shift_then_unshift() {
    Arguments args(state->symbol("blah"), 2, static_args);

    Fixnum* shifted = as<Fixnum>(args.shift(state));
    TS_ASSERT_EQUALS(shifted, Fixnum::from(3));
    TS_ASSERT_EQUALS(args.get_argument(0), Fixnum::from(4));

    args.unshift(state, Fixnum::from(13));

    TS_ASSERT_EQUALS(args.get_argument(0), Fixnum::from(13));
    TS_ASSERT_EQUALS(args.get_argument(1), Fixnum::from(4));
  }

  void test_shift_one_then_unshift_one() {
    Arguments args(state->symbol("blah"), 1, static_args);

    Fixnum* shifted = as<Fixnum>(args.shift(state));
    TS_ASSERT_EQUALS(shifted, Fixnum::from(3));
    TS_ASSERT_EQUALS(0U, args.total());

    args.unshift(state, shifted);

    TS_ASSERT_EQUALS(1U, args.total());

    TS_ASSERT_EQUALS(args.get_argument(0), shifted);
  }

  void test_shift_in_array() {
    Arguments args(state->symbol("blah"), 1, static_args);

    Array* ary = Array::create(state, 1);
    ary->set(state, 0, state->symbol("number_two"));
    args.append(state, ary);

    TS_ASSERT_EQUALS(2U, args.total());

    Fixnum* shifted = as<Fixnum>(args.shift(state));
    TS_ASSERT_EQUALS(shifted, three);
    TS_ASSERT_EQUALS(1U, args.total());

    TS_ASSERT_EQUALS(args.get_argument(0), state->symbol("number_two"));
  }

  void test_shift_one_then_unshift_two() {
    Arguments args(state->symbol("blah"), 1, static_args);

    Fixnum* shifted = as<Fixnum>(args.shift(state));
    TS_ASSERT_EQUALS(shifted, three);
    TS_ASSERT_EQUALS(0U, args.total());

    args.unshift2(state, shifted, Fixnum::from(47));

    TS_ASSERT_EQUALS(2U, args.total());

    TS_ASSERT_EQUALS(args.get_argument(0), shifted);
    TS_ASSERT_EQUALS(args.get_argument(1), Fixnum::from(47));
  }

  void test_as_array() {
    Arguments args(state->symbol("blah"), 1, static_args);

    Array* ary = args.as_array(state);
    TS_ASSERT_EQUALS(ary->size(), 1);
    TS_ASSERT_EQUALS(ary->get(state, 0), three);

    args.unshift(state, four);

    ary = args.as_array(state);
    TS_ASSERT_EQUALS(ary->size(), 2);
    TS_ASSERT_EQUALS(ary->get(state, 0), four);
    TS_ASSERT_EQUALS(ary->get(state, 1), three);

    args.shift(state);

    ary = args.as_array(state);
    TS_ASSERT_EQUALS(ary->size(), 1);
    TS_ASSERT_EQUALS(ary->get(state, 0), three);

  }

  void test_use_array_with_shifted_array() {
    Arguments args(state->symbol("blah"), 1, static_args);

    Array *ary = Array::create(state, 3);
    ary->set(state, 0, Fixnum::from(1));
    ary->set(state, 1, Fixnum::from(2));
    ary->set(state, 2, Fixnum::from(3));
    ary->shift(state);

    args.use_array(ary);
    TS_ASSERT_EQUALS(args.get_argument(0), Fixnum::from(2));
    TS_ASSERT_EQUALS(args.get_argument(1), Fixnum::from(3));
  }
};
