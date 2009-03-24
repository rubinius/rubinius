#include "vm/test/test.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/nativemethod.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

#include "capi_reset.h"

using namespace rubinius;
using namespace capi;

class TestCApiImmediates : public CxxTest::TestSuite, public VMTest {
  public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_Qfalse() {
    TS_ASSERT_EQUALS(cCApiHandleQfalse, cCApiQfalse);
  }

  void test_Qtrue() {
    TS_ASSERT_EQUALS(cCApiHandleQtrue, cCApiQtrue);
  }

  void test_Qnil() {
    TS_ASSERT_EQUALS(cCApiHandleQnil, cCApiQnil);
  }

  void test_Qundef() {
    TS_ASSERT_EQUALS(cCApiHandleQundef, cCApiQundef);
  }
};
