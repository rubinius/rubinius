#ifndef RBX_BUILTIN_THREAD_HPP
#define RBX_BUILTIN_THREAD_HPP

#include "vm/exception.hpp"
#include "vm/type_info.hpp"

#include "builtin/object.hpp"
#include "executor.hpp"

namespace rubinius {

  class Channel;
  class Exception;

  /**
   *  Ruby Thread implementation.
   *
   *  Each Thread is backed by a native thread. This class
   *  provides the interface Ruby expects to see to manipulate
   *  Thread execution.
   */
  class Thread : public Object {
    /** Thread is created and valid and not yet done? */
    Object* alive_;        // slot

    /** Thread is currently sleeping and not running? */
    Object* sleep_;        // slot

    Channel* control_channel_; // slot

    /** LookupTable of objects that contain themselves. */
    LookupTable* recursive_objects_;  // slot

    Thread* debugger_thread_; // slot

    Fixnum* thread_id_; // slot

    thread::SpinLock init_lock_;

    /// The VM state for this thread and this thread alone
    VM* vm_;

    typedef Object* (*Run)(STATE);

    Run runner_;

  public:
    const static object_type type = ThreadType;

    static void   init(VM* state);

  public:
    attr_accessor(alive, Object);

    attr_accessor(sleep, Object);

    attr_accessor(control_channel, Channel);

    attr_accessor(recursive_objects, LookupTable);

    attr_accessor(debugger_thread, Thread);

    attr_accessor(thread_id, Fixnum);

    VM* vm() {
      return vm_;
    }

  public:

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
     *  @see  kernel/bootstrap/thread.rb
     */
    // Rubinius.primitive :thread_allocate
    static Thread* allocate(STATE, Object* self);

    /**
     *  Returns the Thread object for the state.
     *
     *  This is the currently executing Thread.
     */
    // Rubinius.primitive :thread_current
    static Thread* current(STATE);

    /**
     *  Attempt to schedule some other Thread.
     */
    // Rubinius.primitive :thread_pass
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
     *  @see  kernel/bootstrap/thread.rb
     */
    // Rubinius.primitive :thread_fork
    Object* fork(STATE);

    /**
     *  Execute the Thread.
     *
     *  This leaves the thread in an attached state, so that
     *  a pthread_join() later on will work.
     */
    int fork_attached(STATE);

    /**
     *  Retrieve the priority set for this Thread.
     *
     *  The value is numeric, higher being more important
     *  but otherwise *potentially* platform-specific for
     *  any other connotations.
     */
    // Rubinius.primitive :thread_priority
    Object* priority(STATE);

    /**
     *  Process an exception raised for this Thread.
     */
    // Rubinius.primitive :thread_raise
    Object* raise(STATE, Exception* exc);

    /**
     *  Set the priority for this Thread.
     *
     *  The value is numeric, higher being more important
     *  but otherwise *potentially* platform-specific for
     *  any other connotations.
     */
    // Rubinius.primitive :thread_set_priority
    Object* set_priority(STATE, Fixnum* priority);

    /**
     *  Schedule Thread to be run.
     *
     *  This wakes up a sleeping Thread, although it can also
     *  be invoked on an already-running Thread. The Thread
     *  is queued to be run, although not necessarily immediately.
     */
    // Rubinius.primitive :thread_wakeup
    Thread* wakeup(STATE);

    // Rubinius.primitive :thread_context
    Tuple* context(STATE);

    // Rubinius.primitive :thread_join
    Object* join(STATE, CallFrame* calling_environment);

    // Rubinius.primitive :thread_set_critical
    static Object* set_critical(STATE, Object* obj);

    // Rubinius.primitive :thread_unlock_locks
    Object* unlock_locks(STATE);

    void cleanup();

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
    static Thread* create(STATE, VM* target, Object* self, Run runner,
                          bool main_thread = false);

    static void* in_new_thread(void*);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}


#endif
