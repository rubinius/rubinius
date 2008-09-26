#ifndef RBX_THREAD_HPP
#define RBX_THREAD_HPP

#include "vm/exception.hpp"
#include "vm/type_info.hpp"

#include "builtin/object.hpp"

namespace rubinius {
  class Channel;
  class Task;

  class Thread : public Object {
  public:
    const static size_t fields = 5;
    const static object_type type = ThreadType;

  private:
    Task* task_;       // slot
    Channel* channel_; // slot
    FIXNUM priority_;  // slot

  public:
    /* accessors */

    attr_accessor(task, Task);
    attr_accessor(channel, Channel);
    attr_accessor(priority, Fixnum);

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
