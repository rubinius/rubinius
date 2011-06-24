#include "vm/test/test.hpp"
#include "api/embed.h"

class TestEmbedding : public CxxTest::TestSuite, public VMTest {
public:

  void test_create_and_close_context() {
	rbx_ctx ctx = rbx_create_context();
	TS_ASSERT_DIFFERS(NULL, ctx);
	rbx_close_ctx(ctx);
  }
};
