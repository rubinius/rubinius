#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "ffi.hpp"
#include "builtin/contexts.hpp"
#include "context_cache.hpp"

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

  void test_line() {
    MethodContext* ctx = MethodContext::create(state, 10);
    ctx->ip = 0;
    ctx->cm = CompiledMethod::create(state);

    ctx->cm->lines = Tuple::from(state, 1,
                                 Tuple::from(state, 3,
                                             Fixnum::from(0),
                                             Fixnum::from(20),
                                             Fixnum::from(10)));

    TS_ASSERT_EQUALS(10, ctx->line());
  }

  void test_recycle() {
    MethodContext* ctx = MethodContext::create(state, 10);
    state->context_cache->reclaim = 1;
    TS_ASSERT(ctx->recycle(state));

    TS_ASSERT_EQUALS(state->context_cache->reclaim, 0);
    MethodContext* ctx2 = MethodContext::create(state, 10);
    TS_ASSERT_EQUALS(ctx, ctx2);
  }

  void test_recycle_ignores_mature_contexts() {
    MethodContext* ctx = MethodContext::create(state, 10);
    ctx->zone = MatureObjectZone; // GROSS
    state->context_cache->reclaim = 1;
    TS_ASSERT(!ctx->recycle(state));
  }
};
