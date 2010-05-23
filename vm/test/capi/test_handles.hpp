#include "vm/test/test.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/nativemethod.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

#include "capi_reset.h"

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

  void test_Qfalse() {
    TS_ASSERT_EQUALS(0, cCApiQfalse);
    TS_ASSERT_EQUALS(cCApiHandleQfalse, cCApiQfalse);
  }

  void test_Qtrue() {
    TS_ASSERT_EQUALS(0x22, cCApiQtrue)
    TS_ASSERT_EQUALS(cCApiHandleQtrue, cCApiQtrue);
  }

  void test_Qnil() {
    TS_ASSERT_EQUALS(0x42, cCApiQnil)
    TS_ASSERT_EQUALS(cCApiHandleQnil, cCApiQnil);
  }

  void test_Qundef() {
    TS_ASSERT_EQUALS(0x62, cCApiQundef)
    TS_ASSERT_EQUALS(cCApiHandleQundef, cCApiQundef);
  }

  void test_CAPI_FALSE_P() {
    TS_ASSERT(CAPI_FALSE_P(cCApiQfalse));
    TS_ASSERT(!CAPI_FALSE_P(cCApiQtrue));
    TS_ASSERT(!CAPI_FALSE_P(cCApiQnil));
    TS_ASSERT(!CAPI_FALSE_P(cCApiQundef));
  }

  void test_CAPI_TRUE_P() {
    TS_ASSERT(!CAPI_TRUE_P(cCApiQfalse));
    TS_ASSERT(CAPI_TRUE_P(cCApiQtrue));
    TS_ASSERT(!CAPI_TRUE_P(cCApiQnil));
    TS_ASSERT(!CAPI_TRUE_P(cCApiQundef));
  }

  void test_CAPI_NIL_P() {
    TS_ASSERT(!CAPI_NIL_P(cCApiQfalse));
    TS_ASSERT(!CAPI_NIL_P(cCApiQtrue));
    TS_ASSERT(CAPI_NIL_P(cCApiQnil));
    TS_ASSERT(!CAPI_NIL_P(cCApiQundef));
  }

  void test_CAPI_UNDEF_P() {
    TS_ASSERT(!CAPI_UNDEF_P(cCApiQfalse));
    TS_ASSERT(!CAPI_UNDEF_P(cCApiQtrue));
    TS_ASSERT(!CAPI_UNDEF_P(cCApiQnil));
    TS_ASSERT(CAPI_UNDEF_P(cCApiQundef));
  }

  void test_CAPI_GLOBAL_HANDLE_P() {
    VALUE val = CAPI_APPLY_GLOBAL_TAG(3);

    TS_ASSERT(CAPI_GLOBAL_HANDLE_P(val));
    TS_ASSERT(!CAPI_LOCAL_HANDLE_P(val));
  }

  void test_CAPI_LOCAL_HANDLE_P() {
    VALUE val = CAPI_APPLY_LOCAL_TAG(4);

    TS_ASSERT(CAPI_LOCAL_HANDLE_P(val));
    TS_ASSERT(!CAPI_GLOBAL_HANDLE_P(val));
  }
};
