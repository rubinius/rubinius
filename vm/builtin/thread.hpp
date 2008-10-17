#ifndef RBX_THREAD_HPP
#define RBX_THREAD_HPP

#include "vm/exception.hpp"
#include "vm/type_info.hpp"

#include "builtin/object.hpp"
#include "executor.hpp"

namespace rubinius {
  class Channel;
  class Task;
  class Exception;

  class Thread : public Object {
  public:
    const static size_t fields = 6;
    const static object_type type = ThreadType;

  private:
    Task* task_;       // slot
    Channel* channel_; // slot
    FIXNUM priority_;  // slot
    OBJECT alive_;     // slot
    OBJECT sleep_;     // slot
    OBJECT queued_;    // slot

  public:
    /* accessors */

    attr_accessor(task, Task);
    attr_accessor(channel, Channel);
    attr_accessor(priority, Fixnum);
    attr_accessor(sleep, Object);
    attr_accessor(alive, Object);
    attr_accessor(queued, Object);

    /* interface */

    static void init(STATE);

    // Ruby.primitive :thread_new
    static Thread* create(STATE);

    // Ruby.primitive :thread_current
    static Thread* current(STATE);

    void boot_task(STATE);

    // Ruby.primitive :thread_yield
    static Object* pass(STATE);

    // Ruby.primitive :thread_run
    Thread* run(STATE);

    void sleep_for(STATE, Channel* chan);
    void set_top(STATE, OBJECT val);

    // Ruby.primitive :thread_schedule
    Thread* wakeup(STATE);

    // Ruby.primitive :thread_raise
    OBJECT raise(STATE, Exception* exc);

    // Ruby.primitive? :thread_dequeue
    ExecuteStatus dequeue_prim(STATE, Executable* exec, Task* task, Message& msg);

    // Called by the VM when this thread has been schedule to run.
    void woken(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class DeadLock : public Assertion {
  public:
    DeadLock(const char* msg) : Assertion(msg) { }
  };
}

#endif
