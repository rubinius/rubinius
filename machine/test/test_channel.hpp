#include "machine/test/test.hpp"

#include <iostream>

#include "class/channel.hpp"
#include "class/list.hpp"
#include "class/symbol.hpp"
#include "class/thread.hpp"

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
    chan->send(state, cTrue);
    TS_ASSERT_EQUALS(as<List>(chan->value())->size(), 1U);
    chan->send(state, cTrue);
    TS_ASSERT_EQUALS(as<List>(chan->value())->size(), 2U);
  }

  void test_send_then_receive() {
    chan->send(state, cTrue);

    TS_ASSERT(kind_of<List>(chan->value()));
    List* lst = as<List>(chan->value());
    TS_ASSERT_EQUALS(lst->size(), 1U);
    TS_ASSERT_EQUALS(lst->locate(state, 0), cTrue);

    Object* obj = chan->receive(state);
    TS_ASSERT_EQUALS(obj, cTrue);
    TS_ASSERT_EQUALS(lst->size(), 0U);
  }

  void compare_interval_in_range(struct timeval start, struct timeval finish,
				 suseconds_t tmin, suseconds_t tmax) {
    time_t sec = finish.tv_sec - start.tv_sec;
    suseconds_t actual = sec * 1000000 - start.tv_usec + finish.tv_usec;
    TS_ASSERT_LESS_THAN(tmin, actual);
    TS_ASSERT_LESS_THAN(actual,tmax);
  }

};
