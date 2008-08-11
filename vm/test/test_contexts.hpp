#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "ffi.hpp"
#include "builtin/contexts.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestContexts : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    MethodContext* ctx = MethodContext::create(state, 10);
    TS_ASSERT_EQUALS(ctx->obj_type, MContextType);
    TS_ASSERT(ctx->stack_size >= 10);
    TS_ASSERT_EQUALS(ctx->ip, 0);
  }
};
