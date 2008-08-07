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

  void test_wakeup() {
    Thread* cur = Thread::current(state);
    Thread* thr = Thread::create(state);

    int priority = thr->priority->n2i();

    Thread* thr2 = thr->wakeup(state);

    TS_ASSERT_EQUALS(thr, thr2);
    TS_ASSERT_EQUALS(Thread::current(state), cur);

    List* threads = as<List>(state->globals.scheduled_threads->at(priority));
    TS_ASSERT_EQUALS(false, threads->empty_p());

    Thread* thr3 = as<Thread>(threads->shift(state));
    TS_ASSERT_EQUALS(thr, thr3);
  }

};

