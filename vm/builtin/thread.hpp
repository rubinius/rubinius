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
   *  Error class for deadlocks.
   */
  class DeadLock : public Assertion {
  public:
    DeadLock(const char* msg) : Assertion(msg) { }
  };


  /**
   *  Ruby-side Thread implementation.
   *
   *  Each Thread is backed by a native thread. This class
   *  provides the interface Ruby expects to see to manipulate
   *  Thread execution.
   *
   *  @see  vm/native_thread.hpp
   *  @see  vm/util/thread.hpp
   */
  class Thread : public Object {
  private:  /* Instance vars */

    /** Thread is created and valid and not yet done? */
    Object* alive_;        // slot

    /** Thread is currently sleeping and not running? */
    Object* sleep_;        // slot

    Channel* control_channel_; // slot

    LookupTable* recursive_objects_;  // slot

    Thread* debugger_thread_; // slot

    /**
     *  Actual OS backend thread associated with this Thread.
     *
     *  @see Thread::fork()
     */
    NativeThread* native_thread_;

  public:
    const static object_type type = ThreadType;

    /** Register class with the VM. */
    static void   init(VM* state);


  public:   /* Accessors */

    /** Thread created, valid and not yet done? */
    attr_accessor(alive, Object);

    /** Thread alive but not currently running? */
    attr_accessor(sleep, Object);

    attr_accessor(control_channel, Channel);

    /** LookupTable of objects that contain themselves. */
    attr_accessor(recursive_objects, LookupTable);

    attr_accessor(debugger_thread, Thread);

    /** OS thread associated with this Thread, if any. */
    NativeThread* native_thread() {
      return native_thread_;
    }

  public:   /* Class primitives */

    /**
     *  Allocate a Thread object.
     *
     *  Object is in a valid but not running state.
     *  It still assumes that #initialize will be
     *  called to fully set it up. The object is
     *  not yet associated with an actual native
     *  thread.
     *
     *  This method also creates a new VM object
     *  to represent its state.
     *
     *  @see  Thread::fork()
     *  @see  Thread::create()
     *
     *  @see  vm/vm.hpp
     *  @see  kernel/common/thread.rb
     */
    // Ruby.primitive :thread_allocate
    static Thread* allocate(STATE);

    /**
     *  Returns the Thread object for the state.
     *
     *  This is theoretically the currently executing Thread.
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
    static Object* pass(STATE, CallFrame* calling_environment);


  public:   /* Instance primitives */

    /**
     *  Execute the Thread.
     *
     *  Actually creates the native thread and starts it.
     *  The native thread will start executing this Thread's
     *  #__run__ method.
     *
     *  @see  Thread::allocate()
     *
     *  @see  kernel/common/thread.rb
     *  @see  vm/native_thread.hpp
     */
    // Ruby.primitive :thread_fork
    Object* fork(STATE);

    /**
     *  Retrieve the priority set for this Thread.
     *
     *  The value is numeric, higher being more important
     *  but otherwise *potentially* platform-specific for
     *  any other connotations.
     */
    // Ruby.primitive :thread_priority
    Object* priority(STATE);

    /**
     *  Process an exception raised for this Thread.
     */
    // Ruby.primitive :thread_raise
    Object* raise(STATE, Exception* exc);

    /**
     *  Set the priority for this Thread.
     *
     *  The value is numeric, higher being more important
     *  but otherwise *potentially* platform-specific for
     *  any other connotations.
     */
    // Ruby.primitive :thread_set_priority
    Object* set_priority(STATE, Fixnum* priority);

    /**
     *  Schedule Thread to be run.
     *
     *  This wakes up a sleeping Thread, although it can also
     *  be invoked on an already-running Thread. The Thread
     *  is queued to be run, although not necessarily immediately.
     */
    // Ruby.primitive :thread_wakeup
    Thread* wakeup(STATE);

    // Ruby.primitive :thread_context
    Tuple* context(STATE);


  public:   /* Class methods */

    /**
     *  Create a Thread object.
     *
     *  Used by the Thread::allocate() primitive, creates
     *  the Thread object and associates it with the provided
     *  VM state object. The Thread is not yet associated
     *  with a native thread.
     *
     *  @see  Thread::allocate().
     */
    static Thread* create(STATE, VM* target, pthread_t tid = 0);


  public:   /* Instance methods */

    /**
     *  Disassociate the OS thread from this Thread.
     *
     *  Only done once the native thread has completed.
     *
     *  @see  Thread::fork()
     *  @see  NativeThread::perform()
     */
    void detach_native_thread();

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}


#endif
