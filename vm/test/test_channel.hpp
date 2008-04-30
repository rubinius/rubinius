#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestChannel : public CxxTest::TestSuite {
  public:

#undef state
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
    TS_ASSERT_EQUALS(lst->size(), 1);
    TS_ASSERT_EQUALS(lst->locate(state, 0), Qtrue);

    chan->receive(state);
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(task->stack->at(0), Qtrue);
    TS_ASSERT_EQUALS(lst->size(), 0);
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
    Thread* waiter = Thread::create(state);

    state->queue_thread(other);
    state->activate_thread(waiter);
    TS_ASSERT_EQUALS(G(current_thread), waiter);



  }
};
