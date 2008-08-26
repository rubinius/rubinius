#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"
#include "ffi.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestArray : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_aref() {
    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Fixnum::from(1));
    ary->set(state, 1, Fixnum::from(4));

    TS_ASSERT_EQUALS(4,
        as<Fixnum>(ary->aref(state, Fixnum::from(1)))->to_native());
  }

  void test_aset() {
    Array* ary = Array::create(state, 2);
    ary->aset(state, Fixnum::from(1), Fixnum::from(4));

    TS_ASSERT_EQUALS(4, as<Fixnum>(ary->get(state, 1))->to_native());
  }

  void test_get() {
    Array* ary = Array::create(state, 3);
    TS_ASSERT_EQUALS(Qnil, ary->get(state, 5));

    ary->set(state, 0, Qtrue);

    TS_ASSERT_EQUALS(Qtrue, ary->get(state, 0));
  }

  void test_set_resizes() {
    Array* ary = Array::create(state, 3);

    ary->set(state, 0, Fixnum::from(33));
    ary->set(state, 10, Qtrue);

    TS_ASSERT(ary->tuple->field_count > 3);
    TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(33));

  }

  void test_unshift() {
    Array* ary = Array::create(state, 1);
    ary->set(state, 0, Qtrue);
    ary->unshift(state, Qfalse);

    TS_ASSERT_EQUALS(ary->size(), 2);
    TS_ASSERT_EQUALS(ary->get(state, 0), Qfalse);
    TS_ASSERT_EQUALS(ary->get(state, 1), Qtrue);
  }

  void test_includes_p() {
    Array* ary = Array::create(state, 1);
    ary->set(state, 0, Qtrue);

    TS_ASSERT(ary->includes_p(state, Qtrue));
    TS_ASSERT(!ary->includes_p(state, Qfalse));
  }
};
