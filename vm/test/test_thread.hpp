#include "builtin/thread.hpp"

using namespace rubinius;

class TestThread : public CxxTest::TestSuite {
public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_current() {
    OBJECT current_thread = state->globals.current_thread.get();

    TS_ASSERT_EQUALS(Thread::current(state), current_thread);
  }

  void test_run() {
    Thread* thr = Thread::create(state);

    Thread* thr2 = thr->run(state);

    TS_ASSERT_EQUALS(thr, thr2);
    TS_ASSERT_EQUALS(Thread::current(state), thr);
  }

};

