#include "builtin/channel.hpp"
#include "builtin/list.hpp"
#include "builtin/symbol.hpp"
#include "builtin/thread.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"
#include "event.hpp"

#include <cxxtest/TestSuite.h>
#include <sys/time.h>

using namespace rubinius;

class TestChannel : public CxxTest::TestSuite {
  public:

  VM* state;
  Channel* chan;

  void setUp() {
    state = new VM();
    chan = Channel::create(state);
  }

  void tearDown() {
    delete state;
  }

  void test_channel_fields() {
    TS_ASSERT_EQUALS(2U, Channel::fields);
  }

  void test_create() {
    TS_ASSERT(kind_of<Channel>(chan));
    TS_ASSERT(kind_of<List>(chan->waiting()));
  }

  void test_send_then_receive() {
    Task* task = Task::create(state, 10);

    state->globals.current_task.set(task);

    TS_ASSERT(chan->value()->nil_p());
    chan->send(state, Qtrue);

    TS_ASSERT(kind_of<List>(chan->value()));
    List* lst = as<List>(chan->value());
    TS_ASSERT_EQUALS(lst->size(), 1U);
    TS_ASSERT_EQUALS(lst->locate(state, 0), Qtrue);

    chan->receive(state);
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qtrue);
    TS_ASSERT_EQUALS(lst->size(), 0U);
  }

  void test_receive_causes_deadlock() {
    Task* task = Task::create(state, 10);

    state->globals.current_task.set(task);

    Thread* thread = Thread::create(state);
    state->globals.current_thread.set(thread);

    bool thrown = false;
    try {
      chan->receive(state);
    } catch(DeadLock& e) {
      thrown = true;
    }

    TS_ASSERT(thrown);
  }

  void test_receive_causes_thread_switch() {
    Thread* other = Thread::create(state);

    Thread* orig = G(current_thread);

    state->queue_thread(other);

    chan->receive(state);
    TS_ASSERT_EQUALS(G(current_thread), other);
    TS_ASSERT_EQUALS(orig->sleep(), Qtrue);
  }

  void test_receive_causes_event_block() {
    ChannelCallback cb(state, chan);
    event::Timer* timer = new event::Timer(state, &cb, 1.0);

    Thread* orig = G(current_thread);
    state->events->start(timer);

    OBJECT* stack = G(current_task)->current_stack();

    TS_ASSERT(!state->interrupts.check_events);
    chan->receive(state);
    TS_ASSERT(state->interrupts.check_events);

    TS_ASSERT_EQUALS(chan->waiting()->locate(state, 0), G(current_thread));

    state->events->run_and_wait();

    TS_ASSERT(chan->waiting()->empty_p());

    TS_ASSERT_EQUALS(G(current_thread), orig);
    TS_ASSERT_EQUALS(G(current_task)->calculate_sp(), 0);
    TS_ASSERT_EQUALS(stack[0], Qnil);
  }

  void test_receive_polls_events() {
    ChannelCallback cb(state, chan);
    event::Timer* timer = new event::Timer(state, &cb, 0.2);

    Thread* orig = G(current_thread);
    state->events->start(timer);

    OBJECT* stack = G(current_task)->current_stack();

    TS_ASSERT(!state->interrupts.check_events);
    usleep(300000);
    chan->receive(state);
    TS_ASSERT(!state->interrupts.check_events);

    TS_ASSERT(chan->waiting()->empty_p());

    TS_ASSERT_EQUALS(G(current_thread), orig);
    TS_ASSERT_EQUALS(G(current_task)->calculate_sp(), 0);
    TS_ASSERT_EQUALS(stack[0], Qnil);
  }

  void test_has_readers_p() {
    TS_ASSERT(!chan->has_readers_p());
    chan->waiting()->append(state, G(current_thread));
    TS_ASSERT(chan->has_readers_p());
  }

  void compare_interval_in_range(struct timeval start, struct timeval finish, 
				 suseconds_t tmin, suseconds_t tmax) {
    time_t sec = finish.tv_sec - start.tv_sec;
    suseconds_t actual = sec * 1000000 - start.tv_usec + finish.tv_usec;
    TS_ASSERT_LESS_THAN(tmin, actual);
    TS_ASSERT_LESS_THAN(actual,tmax);
  }

  void test_send_in_microseconds() {
    struct timeval start, finish;

    Thread* orig = G(current_thread);
    SYMBOL done = state->symbol("done");
    gettimeofday(&start, NULL);

    OBJECT* stack = G(current_task)->current_stack();

    OBJECT ret = Channel::send_in_microseconds(state, chan, Fixnum::from(100000), done);
    usleep(200000);
    chan->receive(state);

    gettimeofday(&finish, NULL);
    TS_ASSERT(ret->nil_p());
    TS_ASSERT_EQUALS(G(current_thread), orig);
    TS_ASSERT_EQUALS(done, stack[0]);
    compare_interval_in_range(start,finish,200000U,250000U);
  }

  void test_send_in_seconds() {
    Float* point_one = Float::create(state, 0.1);
    struct timeval start, finish;

    Thread* orig = G(current_thread);
    SYMBOL done = state->symbol("done");
    gettimeofday(&start, NULL);

    OBJECT* stack = G(current_task)->current_stack();

    OBJECT ret = Channel::send_in_seconds(state, chan, point_one, done);
    usleep(201000);
    chan->receive(state);

    gettimeofday(&finish, NULL);
    TS_ASSERT(ret->nil_p());
    TS_ASSERT_EQUALS(G(current_thread), orig);
    TS_ASSERT_EQUALS(done, stack[0]);
    compare_interval_in_range(start, finish, 201000U, 251000U);
  }
};
