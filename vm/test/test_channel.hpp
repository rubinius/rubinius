#include "vm/test/test.hpp"

#include <iostream>

#include "builtin/channel.hpp"
#include "builtin/list.hpp"
#include "builtin/symbol.hpp"
#include "builtin/thread.hpp"

#include <sys/time.h>

class TestChannel : public CxxTest::TestSuite, public VMTest {
  public:

  Channel* chan;

  void setUp() {
    create();
    chan = Channel::create(state);
  }

  void tearDown() {
    destroy();
  }

  void test_create() {
    TS_ASSERT(kind_of<Channel>(chan));
  }

  void test_send_queues_values() {
    chan->send(state, Qtrue);
    TS_ASSERT_EQUALS(as<List>(chan->value())->size(), 1U);
    chan->send(state, Qtrue);
    TS_ASSERT_EQUALS(as<List>(chan->value())->size(), 2U);
  }

  void test_send_then_receive() {
    chan->send(state, Qtrue);

    TS_ASSERT(kind_of<List>(chan->value()));
    List* lst = as<List>(chan->value());
    TS_ASSERT_EQUALS(lst->size(), 1U);
    TS_ASSERT_EQUALS(lst->locate(state, 0), Qtrue);

    CallFrame cf;
    Object* obj = chan->receive(state, &cf);
    TS_ASSERT_EQUALS(obj, Qtrue);
    TS_ASSERT_EQUALS(lst->size(), 0U);
  }

  void test_receive_with_frozen_stack() {
    /*
    Task* task = Task::create(state, 10);
    state->globals.current_task.set(task);

    Thread* thread = Thread::create(state);
    state->globals.current_thread.set(thread);
    thread->frozen_stack(state, Qtrue);

    task->push(Qfalse);
    chan->send(state, Qtrue);
    chan->receive(state);
    TS_ASSERT_EQUALS(task->calculate_sp(), 0);
    TS_ASSERT_EQUALS(task->stack_top(), Qfalse);
    */
  }

  void compare_interval_in_range(struct timeval start, struct timeval finish, 
				 suseconds_t tmin, suseconds_t tmax) {
    time_t sec = finish.tv_sec - start.tv_sec;
    suseconds_t actual = sec * 1000000 - start.tv_usec + finish.tv_usec;
    TS_ASSERT_LESS_THAN(tmin, actual);
    TS_ASSERT_LESS_THAN(actual,tmax);
  }

  void test_send_in_microseconds() {
    /*
    struct timeval start, finish;

    Thread* orig = G(current_thread);
    Symbol* done = state->symbol("done");
    gettimeofday(&start, NULL);

    Object** stack = G(current_task)->current_stack();

    Object* ret = Channel::send_in_microseconds(state, chan, Fixnum::from(100000), done);
    usleep(200000);
    chan->receive(state);

    gettimeofday(&finish, NULL);
    TS_ASSERT(ret->nil_p());
    TS_ASSERT_EQUALS(G(current_thread), orig);
    TS_ASSERT_EQUALS(done, stack[0]);
    compare_interval_in_range(start,finish,200000U,250000U);
    */
  }

  void test_send_in_seconds() {
    /*
    Float* point_one = Float::create(state, 0.1);
    struct timeval start, finish;

    Thread* orig = G(current_thread);
    Symbol* done = state->symbol("done");
    gettimeofday(&start, NULL);

    Object** stack = G(current_task)->current_stack();

    Object* ret = Channel::send_in_seconds(state, chan, point_one, done);
    usleep(201000);
    chan->receive(state);

    gettimeofday(&finish, NULL);
    TS_ASSERT(ret->nil_p());
    TS_ASSERT_EQUALS(G(current_thread), orig);
    TS_ASSERT_EQUALS(done, stack[0]);
    compare_interval_in_range(start, finish, 201000U, 251000U);
    */
  }
};
