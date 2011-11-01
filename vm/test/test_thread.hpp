#include "vm/test/test.hpp"

#include "builtin/channel.hpp"
#include "builtin/list.hpp"
#include "builtin/thread.hpp"

class TestThread : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_current() {
    TS_ASSERT_EQUALS(Thread::current(state), state->vm()->thread.get());
  }

  void test_create() {
    Thread* thr = Thread::create(state, new_vm(), G(thread), 0);

    TS_ASSERT_DIFFERS(thr, Thread::current(state));
  }

  // TODO test wakeup!
};

