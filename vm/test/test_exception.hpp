#include "vm/test/test.hpp"

#include "builtin/exception.hpp"
#include "object_types.hpp"
#include "exception.hpp"

#include <errno.h>

class TestException : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_type_error_raise() {
    object_type type = ClassType;
    Object* obj = G(object);

    TS_ASSERT_THROWS(TypeError::raise(type, obj, "failed"), const TypeError &);
  }

  void test_type_error_raise_without_reason() {
    object_type type = ClassType;
    Object* obj = G(object);

    TS_ASSERT_THROWS(TypeError::raise(type, obj), const TypeError &);
  }

  void test_assertion_raise() {
    TS_ASSERT_THROWS(Assertion::raise("failed"), const Assertion &);
  }

  void test_ruby_exception_argument_error() {
    TS_ASSERT_THROWS_ASSERT(Exception::argument_error(state, 1, 2),
        const RubyException &e,
        TS_ASSERT(Exception::argument_error_p(state, e.exception)));
    TS_ASSERT_THROWS_ASSERT(Exception::argument_error(state, "failed"),
        const RubyException &e,
        TS_ASSERT(Exception::argument_error_p(state, e.exception)));
  }

  void test_make_type_error() {
    object_type type = ClassType;
    Object* obj = G(object);

    TS_ASSERT(Exception::type_error_p(state,
          Exception::make_type_error(state, type, obj)));
  }

  void test_ruby_exception_float_domain_error() {
    TS_ASSERT_THROWS_ASSERT(Exception::float_domain_error(state, "failed"),
        const RubyException &e,
        TS_ASSERT(Exception::float_domain_error_p(state, e.exception)));
  }

  void test_ruby_exception_zero_division_error() {
    TS_ASSERT_THROWS_ASSERT(Exception::zero_division_error(state, "failed"),
        const RubyException &e,
        TS_ASSERT(Exception::zero_division_error_p(state, e.exception)));
  }

  void test_ruby_exception_assertion_error() {
    TS_ASSERT_THROWS_ASSERT(Exception::assertion_error(state, "failed"),
        const RubyException &e,
        TS_ASSERT(Exception::assertion_error_p(state, e.exception)));
  }

  void test_ruby_exception_object_bounds_exceeded_error() {
    TS_ASSERT_THROWS_ASSERT(
        Exception::object_bounds_exceeded_error(state, G(object), 1U),
        const RubyException &e,
        TS_ASSERT(Exception::object_bounds_exceeded_error_p(state, e.exception)));
  }

  void test_ruby_exception_errno_error() {
    TS_ASSERT_THROWS_ASSERT(Exception::errno_error(state, "failed", ENOENT),
        const RubyException &e,
        TS_ASSERT(Exception::errno_error_p(state, e.exception)));
  }

  void test_ruby_exception_errno_error_sets_errno_ivar() {
    TS_ASSERT_THROWS_ASSERT(Exception::errno_error(state, "failed", ENOENT),
        const RubyException &e,
        TS_ASSERT_EQUALS(Fixnum::from(ENOENT),
                         e.exception->get_ivar(state, state->symbol("@errno"))));
  }

  void test_ruby_exception_errno_error_raises_system_call_error() {
    TS_ASSERT_THROWS_ASSERT(Exception::errno_error(state, "failed", -1),
        const RubyException &e,
        TS_ASSERT(Exception::system_call_error_p(state, e.exception)));
  }

  void test_get_errno_error_invalid_errno() {
    TS_ASSERT_EQUALS(Qnil, Exception::get_errno_error(state, Fixnum::from(-1)));
  }

  void test_ruby_exception_io_error() {
    TS_ASSERT_THROWS_ASSERT(Exception::io_error(state, "failed"),
        const RubyException &e,
        TS_ASSERT(Exception::io_error_p(state, e.exception)));
  }

  void test_ruby_exception_runtime_error() {
    TS_ASSERT_THROWS_ASSERT(Exception::runtime_error(state, "failed"),
        const RubyException &e,
        TS_ASSERT(Exception::runtime_error_p(state, e.exception)));
  }
};
