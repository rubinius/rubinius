#include "vm/test/test.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/nativemethod.hpp"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

using namespace rubinius;
using namespace capi;

class TestCApiHandles : public CxxTest::TestSuite, public VMTest {
  public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_cFalse() {
    TS_ASSERT_EQUALS(0, Qfalse);
  }

  void test_cTrue() {
    TS_ASSERT_EQUALS(0x22, Qtrue)
  }

  void test_cNil() {
    TS_ASSERT_EQUALS(0x42, Qnil)
  }

  void test_cUndef() {
    TS_ASSERT_EQUALS(0x62, Qundef)
  }

  void test_FALSE_P() {
    TS_ASSERT(FALSE_P(Qfalse));
    TS_ASSERT(!FALSE_P(Qtrue));
    TS_ASSERT(!FALSE_P(Qnil));
    TS_ASSERT(!FALSE_P(Qundef));
  }

  void test_TRUE_P() {
    TS_ASSERT(!TRUE_P(Qfalse));
    TS_ASSERT(TRUE_P(Qtrue));
    TS_ASSERT(!TRUE_P(Qnil));
    TS_ASSERT(!TRUE_P(Qundef));
  }

  void test_NIL_P() {
    TS_ASSERT(!NIL_P(Qfalse));
    TS_ASSERT(!NIL_P(Qtrue));
    TS_ASSERT(NIL_P(Qnil));
    TS_ASSERT(!NIL_P(Qundef));
  }

  void test_UNDEF_P() {
    TS_ASSERT(!UNDEF_P(Qfalse));
    TS_ASSERT(!UNDEF_P(Qtrue));
    TS_ASSERT(!UNDEF_P(Qnil));
    TS_ASSERT(UNDEF_P(Qundef));
  }
};
