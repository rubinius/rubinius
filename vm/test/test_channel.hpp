#include "objects.hpp"
#include "builtin/list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "event.hpp"

#include <cxxtest/TestSuite.h>

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

  void test_create() {
    TS_ASSERT(kind_of<Channel>(chan));
    TS_ASSERT(kind_of<List>(chan->waiting));
  }

  void test_send_then_receive() {
    Task* task = Task::create(state);
    SET(task, stack, Tuple::create(state, 10));

    state->globals.current_task.set(task);

    TS_ASSERT(chan->value->nil_p());
    chan->send(state, Qtrue);

    TS_ASSERT(kind_of<List>(chan->value));
    List* lst = as<List>(chan->value);
    TS_ASSERT_EQUALS(lst->size(), 1U);
    TS_ASSERT_EQUALS(lst->locate(state, 0), Qtrue);

    chan->receive(state);
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(task->stack->at(0), Qtrue);
    TS_ASSERT_EQUALS(lst->size(), 0U);
  }

  void test_receive_causes_deadlock() {
    Task* task = Task::create(state);
    SET(task, stack, Tuple::create(state, 10));

    state->globals.current_task.set(task);

    Thread* thread = Thread::create(state);
    state->globals.current_thread.set(thread);

    bool thrown = false;
    try {
      chan->receive(state);
    } catch(DeadLock* e) {
      thrown = true;
    }

    TS_ASSERT(thrown);
  }

  void test_receive_causes_thread_switch() {
    Thread* other = Thread::create(state);

    Thread* orig = G(current_thread);

    G(current_task)->stack = Tuple::create(state, 10);
    state->queue_thread(other);

    chan->receive(state);
    TS_ASSERT_EQUALS(G(current_thread), other);
    TS_ASSERT_EQUALS(orig->get_ivar(state, state->symbol("@sleep")), Qtrue);
  }

  void test_receive_causes_event_block() {
    ChannelCallback cb(state, chan);
    event::Timer* timer = new event::Timer(state, &cb, 0.2);

    Thread* orig = G(current_thread);
    state->events->start(timer);

    G(current_task)->stack = Tuple::create(state, 10);

    TS_ASSERT(!state->wait_events);
    chan->receive(state);

    TS_ASSERT_EQUALS(chan->waiting->locate(state, 0), G(current_thread));

    TS_ASSERT(state->wait_events);
    state->events->run_and_wait();

    TS_ASSERT(chan->waiting->empty_p());

    TS_ASSERT_EQUALS(G(current_thread), orig);
    TS_ASSERT_EQUALS(G(current_task)->sp, 0);
    TS_ASSERT_EQUALS(G(current_task)->stack->at(0), Qnil)
  }

  void test_receive_polls_events() {
    ChannelCallback cb(state, chan);
    event::Timer* timer = new event::Timer(state, &cb, 0.2);

    Thread* orig = G(current_thread);
    state->events->start(timer);

    G(current_task)->stack = Tuple::create(state, 10);

    TS_ASSERT(!state->wait_events);
    usleep(300000);
    chan->receive(state);
    TS_ASSERT(!state->wait_events);
    
    TS_ASSERT(chan->waiting->empty_p());

    TS_ASSERT_EQUALS(G(current_thread), orig);
    TS_ASSERT_EQUALS(G(current_task)->sp, 0);
    TS_ASSERT_EQUALS(G(current_task)->stack->at(0), Qnil)
  }

  void test_has_readers_p() {
    TS_ASSERT(!chan->has_readers_p());
    chan->waiting->append(state, G(current_thread));
    TS_ASSERT(chan->has_readers_p());
  }
};
