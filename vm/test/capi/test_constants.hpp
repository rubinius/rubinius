#include "vm/test/test.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/nativemethod.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

#include "capi_reset.h"

using namespace rubinius;
using namespace capi;

class TestCApiConstants : public CxxTest::TestSuite, public VMTest {
  public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_immediate_values() {
    TS_ASSERT_EQUALS(cCApiHandleQfalse, cCApiQfalse);
    TS_ASSERT_EQUALS(cCApiHandleQtrue, cCApiQtrue);
    TS_ASSERT_EQUALS(cCApiHandleQnil, cCApiQnil);
    TS_ASSERT_EQUALS(cCApiHandleQundef, cCApiQundef);
  }

  Object* get_object(Handle handle) {
    NativeMethodEnvironment* nme = NativeMethodEnvironment::get();;
    return nme->get_object(handle);
  }

  void notest_get_Object() {
    TS_ASSERT_EQUALS(G(object), get_object(capi_get_constant(cCApiObject)));
  }

  void notest_get_Array() {
    TS_ASSERT_EQUALS(G(array), get_object(capi_get_constant(cCApiArray)));
  }
};
