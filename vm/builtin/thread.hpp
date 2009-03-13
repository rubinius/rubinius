#ifndef RBX_BUILTIN_THREAD_HPP
#define RBX_BUILTIN_THREAD_HPP

#include "vm/exception.hpp"
#include "vm/type_info.hpp"

#include "builtin/object.hpp"
#include "executor.hpp"

namespace rubinius {

  class Channel;
  class Exception;
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
    attr_accessor(queued, Object);
    attr_accessor(sleep, Object);
    attr_accessor(frozen_stack, Object);

    NativeThread* native_thread() {
      return native_thread_;
    }

    void detach_native_thread() {
      native_thread_ = NULL;
    }

  public:   /* Primitives */

    // Ruby.primitive :thread_allocate
    static Thread* allocate(STATE);

    static Thread* create(STATE, VM* target);

    // Ruby.primitive :thread_fork
    Object* fork(STATE);

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


  private:  /* Instance vars */

    Object*   alive_;        // slot
    Object*   sleep_;        // slot
    Object*   queued_;       // slot
    Object*   frozen_stack_; // slot

    // The VM class that represents this running thread
    VM*       vm;

    // Once the thread is actually forked, this is set assigned to
    // and removed once the thread ends.
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
