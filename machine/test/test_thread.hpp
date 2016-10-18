#include "machine/test/test.hpp"

#include "class/channel.hpp"
#include "class/list.hpp"
#include "class/thread.hpp"

class TestThread : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_current() {
    TS_ASSERT_EQUALS(Thread::current(state), state->vm()->thread());
  }

  void test_create() {
    Thread* thr = Thread::create(state, shared->thread_nexus()->new_vm(shared));

    TS_ASSERT_DIFFERS(thr, Thread::current(state));
  }

  // TODO test wakeup!
};

