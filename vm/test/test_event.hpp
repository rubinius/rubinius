#include "event.hpp"
#include "builtin/io.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"

#include <unistd.h>
#include <signal.h>
#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestChannelObject : public ObjectCallback {
public:
  bool called;
  OBJECT value;

  TestChannelObject(STATE) : ObjectCallback(state), called(false) { }

  virtual OBJECT object() { return Qnil; }

  virtual void call(OBJECT obj) {
    called = true;
    value = obj;
  }
};

class TestEventLoop : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }
  
  void test_timer() {
    TestChannelObject chan(state);
    event::Timer* timer = new event::Timer(state, &chan, 0.2);

    state->events->start(timer);

    TS_ASSERT_EQUALS(timer->loop, state->events);
    TS_ASSERT(!chan.called);
    state->events->run_and_wait();
    TS_ASSERT(chan.called);
    TS_ASSERT_EQUALS(chan.value, Qnil);
  }

  void test_io_read() {
    int fds[2];
    TS_ASSERT(!pipe(fds));

    TestChannelObject chan(state);
    event::Read* read = new event::Read(state, &chan, fds[0]);

    state->events->start(read);
    TS_ASSERT(!chan.called);
    state->events->poll();
    TS_ASSERT(!chan.called);
    write(fds[1], "!", 1);
    state->events->poll();
    TS_ASSERT(chan.called);
    TS_ASSERT_EQUALS(chan.value, Object::i2n(fds[0]));

    close(fds[0]);
    close(fds[1]);
  }
 
  void test_io_read_into_buffer() {
    int fds[2];
    TS_ASSERT(!pipe(fds));

    TestChannelObject chan(state);
    event::Read* read = new event::Read(state, &chan, fds[0]);
    
    IOBuffer *buf = IOBuffer::create(state, 12);
    read->into_buffer(buf, 1);
    TS_ASSERT_EQUALS(buf->used, Object::i2n(0));

    state->events->start(read);
    TS_ASSERT(!chan.called);
    state->events->poll();
    TS_ASSERT(!chan.called);
    
    /* Write some garbage into the buffer to be sure it's overridden */
    char* str = buf->byte_address();
    str[0] = str[1] = 47;

    write(fds[1], "!", 1);
    state->events->poll();
    TS_ASSERT(chan.called);
    TS_ASSERT_EQUALS(chan.value, Object::i2n(1));
    TS_ASSERT_EQUALS(buf->used, Object::i2n(1));

    TS_ASSERT_EQUALS(str[0], '!');
    TS_ASSERT_EQUALS(str[1], 0);
    
    close(fds[0]);
    close(fds[1]);
  }

  void test_signal() {
    event::Loop loop(ev_default_loop(0));
    TestChannelObject chan(state);
    event::Signal* sig = new event::Signal(state, &chan, SIGUSR1);
    loop.start(sig);

    loop.poll();
    TS_ASSERT(!chan.called);

    kill(getpid(), SIGUSR1);
    loop.poll();

    TS_ASSERT(chan.called);
    sig->stop();
  }
};
