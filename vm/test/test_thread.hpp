#include "builtin/channel.hpp"
#include "builtin/list.hpp"
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

  void test_thread_fileds() {
    TS_ASSERT_EQUALS(6U, Thread::fields);
  }

  void test_current() {
    Object* current_thread = state->globals.current_thread.get();

    TS_ASSERT_EQUALS(Thread::current(state), current_thread);
  }

  void test_create() {
    Thread* thr = Thread::create(state);

    TS_ASSERT_EQUALS(2, thr->priority()->to_native());
    TS_ASSERT_DIFFERS(thr, Thread::current(state));
  }

  void test_exited() {
    Thread* cur = Thread::current(state);
    Thread* thr = Thread::create(state);
    Thread* thr2 = Thread::create(state);

    thr->wakeup(state);
    thr2->wakeup(state);

    state->queue_thread(thr);
    state->activate_thread(thr2);


    TS_ASSERT_EQUALS(thr2, Thread::current(state));
    TS_ASSERT_EQUALS(Qtrue, thr2->alive());

    TS_ASSERT_EQUALS(Qtrue, thr->alive());
    TS_ASSERT_EQUALS(Qfalse, thr->sleep());
    TS_ASSERT_EQUALS(Qtrue, thr->queued());

    thr->exited(state);

    TS_ASSERT_EQUALS(thr2, Thread::current(state));

    TS_ASSERT_EQUALS(Qfalse, thr->alive());
    TS_ASSERT_EQUALS(Qfalse, thr->queued());

    thr2->exited(state);

    TS_ASSERT_EQUALS(cur, Thread::current(state));

    TS_ASSERT_EQUALS(Qfalse, thr->alive());
    TS_ASSERT_EQUALS(Qfalse, thr->queued());
  }

  void test_exited_cancels_channel_wait() {
    Message fake_message(state);

    Thread* current = Thread::current(state);
    Channel* channel = Channel::create(state);

    /* Avoid deadlock-ish */
    state->queue_thread(Thread::create(state));

    (void) channel->receive_prim(state, NULL, current->task(), fake_message);

    List* list = as<List>(channel->waiting());
    TS_ASSERT_EQUALS(current, as<Thread>(list->locate(state, 0)));

    current->exited(state);

    TS_ASSERT_EQUALS(0LL, list->count()->to_long_long());
  }

  void test_pass() {
    Thread* thr = Thread::create(state);
    thr->wakeup(state);

    Object* obj = thr->pass(state);

    TS_ASSERT_EQUALS(Qnil, obj);
    TS_ASSERT_EQUALS(thr, Thread::current(state));
  }

  void test_wakeup() {
    Thread* thr = Thread::create(state);
    thr->sleep(state, Qtrue);

    Thread* thr2 = thr->wakeup(state);

    TS_ASSERT_EQUALS(thr, thr2);

    TS_ASSERT_EQUALS(thr->sleep(), Qfalse);
    TS_ASSERT_EQUALS(thr->queued(), Qtrue);
  }

  void test_wakeup_cancels_channel_wait() {
    Message fake_message(state);

    Thread* current = Thread::current(state);
    Channel* channel = Channel::create(state);

    /* Avoid deadlock-ish */
    state->queue_thread(Thread::create(state));

    (void) channel->receive_prim(state, NULL, current->task(), fake_message);

    List* list = as<List>(channel->waiting());
    TS_ASSERT_EQUALS(current, as<Thread>(list->locate(state, 0)));

    current->wakeup(state);

    TS_ASSERT_EQUALS(0LL, list->count()->to_long_long());
  }

  void test_raise_dead() {
    Thread* thr = Thread::create(state);
    thr->alive(state, Qfalse);

    TS_ASSERT_EQUALS(reinterpret_cast<Object*>(kPrimitiveFailed),
                     reinterpret_cast<Object*>(thr->wakeup(state)));
  }

  void test_enlarge_for_priority() {
    std::size_t current_max = state->globals.scheduled_threads->num_fields();

    Thread* thread = Thread::create(state);


    TS_ASSERT_EQUALS(state->globals.scheduled_threads->num_fields(),
                     current_max);

    thread->priority(state, Fixnum::from(current_max));

    TS_ASSERT_EQUALS(state->globals.scheduled_threads->num_fields(),
                     (current_max + 1));

    thread->priority(state, Fixnum::from(1));

    TS_ASSERT_EQUALS(state->globals.scheduled_threads->num_fields(),
                     (current_max + 1));

    thread->priority(state, Fixnum::from(current_max + 10));

    TS_ASSERT_EQUALS(state->globals.scheduled_threads->num_fields(),
                     (current_max + 11));
  }

};

