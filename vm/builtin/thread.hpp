#ifndef RBX_BUILTIN_THREAD_HPP
#define RBX_BUILTIN_THREAD_HPP

#include "vm/exception.hpp"
#include "vm/type_info.hpp"

#include "builtin/object.hpp"
#include "executor.hpp"

namespace rubinius {

  class Channel;
  class Exception;
  class Task;
  class NativeThread;

  /**
   *  Ruby's green Thread backend.
   *
   *  Thread is the basic unit of concurrency for Ruby
   *  code. The threads are not truly parallel and run
   *  co-operatively at the system level. Pre-emption
   *  is enabled by default but the switching logic is
   *  the same as for an explicit schedule.
   */
  class Thread : public Object {
  public:
    const static object_type type = ThreadType;

    /** Register class with the VM. */
    static void init(STATE);


  public:   /* Accessors */

    attr_accessor(alive, Object);
    attr_accessor(channel, Channel);
    attr_accessor(queued, Object);
    attr_accessor(sleep, Object);
    attr_accessor(task, Task);
    attr_accessor(frozen_stack, Object);

    NativeThread* native_thread() {
      return native_thread_;
    }

  public:   /* Primitives */

    // Ruby.primitive :thread_new
    static Thread* s_new(STATE, Message& msg);

    static Thread* create(STATE, VM* target);

    /**
     *  Returns the currently executing Thread.
     */
    // Ruby.primitive :thread_current
    static Thread* current(STATE);

    /**
     *  Attempt to schedule some other Thread.
     *
     *  The other Thread, if found, is set as the active
     *  one but this only goes in effect once this pass
     *  is over.
     */
    // Ruby.primitive :thread_pass
    static Object* pass(STATE);

    // Ruby.primitive :thread_sleep
    static Object* sleep_now(STATE, Object* duration);

    /**
     *  Raise exception in this Thread.
     *
     *  The Thread is woken first, then the given exception
     *  is raised so that the Thread will proceed with exception
     *  handling when it is scheduled (which may not be immediately.)
     */
    // Ruby.primitive :thread_raise
    Object* raise(STATE, Exception* exc);

    /**
     *  Schedule Thread to be run.
     *
     *  This wakes up a sleeping Thread, although it can also
     *  be invoked on an already-running Thread. The Thread
     *  is queued to be run, although not necessarily immediately.
     */
    // Ruby.primitive :thread_wakeup
    Thread* wakeup(STATE);

    // Ruby.primitive :thread_priority
    Object* priority(STATE);

    // Ruby.primitive :thread_set_priority
    Object* set_priority(STATE, Fixnum* priority);


  public:   /* Interface */

    /** Create a Task and associate it with this Thread. */
    void boot_task(STATE);

    /** Set object as the top of the active stack. */
    void set_top(STATE, Object* val);

    /** Have this Thread wait to receive from the Channel. */
    void sleep_for(STATE, Channel* chan);


  private:  /* Instance vars */

    Task*     task_;         // slot
    Channel*  channel_;      // slot
    Object*   alive_;        // slot
    Object*   sleep_;        // slot
    Object*   queued_;       // slot
    Object*   frozen_stack_; // slot

    // The VM class that represents this running thread
    VM*       vm;
    NativeThread* native_thread_;


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
