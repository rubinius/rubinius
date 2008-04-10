#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "ffi.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestArray : public CxxTest::TestSuite {
  public:

#undef state
  VM *state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_get() {
    Array* ary = Array::create(state, 3);
    TS_ASSERT_EQUALS(Qnil, ary->get(state, 5));

    ary->set(state, 0, Qtrue);

    TS_ASSERT_EQUALS(Qtrue, ary->get(state, 0));
  }

  void test_set_resizes() {
    Array* ary = Array::create(state, 3);

    ary->set(state, 0, Object::i2n(33));
    ary->set(state, 10, Qtrue);

    TS_ASSERT(ary->tuple->field_count > 3);
    TS_ASSERT_EQUALS(ary->get(state, 0), Object::i2n(33));

  }
};
