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
    Task*     task_;      // slot
    Channel*  channel_;   // slot
    Fixnum*   priority_;  // slot
    Object*   alive_;     // slot
    Object*   sleep_;     // slot
    Object*   queued_;    // slot

  public:
    /* accessors */

    attr_accessor(task, Task);
    attr_accessor(channel, Channel);
    attr_accessor(priority, Fixnum);
    attr_accessor(sleep, Object);
    attr_accessor(alive, Object);
    attr_accessor(queued, Object);


  public:   /* Interface */


    static void init(STATE);

    // Ruby.primitive :thread_allocate
    static Thread* create(STATE);

    // Ruby.primitive :thread_current
    static Thread* current(STATE);

    // Ruby.primitive :thread_exited
    Object* exited(STATE);

    // Ruby.primitive :thread_pass
    Object* pass(STATE);

    // Ruby.primitive :thread_raise
    Object* raise(STATE, Exception* exc);

    /** Schedule to run again (not necessarily right now.) */
    // Ruby.primitive :thread_wakeup
    Thread* wakeup(STATE);

    void boot_task(STATE);
    void set_top(STATE, Object* val);
    void sleep_for(STATE, Channel* chan);


    void woken(STATE);


  public:   /* TypeInfo */

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
