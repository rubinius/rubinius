#ifndef RBX_BUILTIN_THREAD_HPP
#define RBX_BUILTIN_THREAD_HPP

#include "object_utils.hpp"

#include "builtin/channel.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookup_table.hpp"
#include "builtin/object.hpp"
#include "builtin/randomizer.hpp"

namespace rubinius {
  class Array;

  /**
   *  Ruby Thread implementation.
   *
   *  Each Thread is backed by a native thread. This class
   *  provides the interface Ruby expects to see to manipulate
   *  Thread execution.
   */
  class Thread : public Object {
    Array* args_;                     // slot
    Object* block_;                   // slot
    Object* alive_;                   // slot
    Object* sleep_;                   // slot
    Channel* control_channel_;        // slot
    LookupTable* recursive_objects_;  // slot
    Thread* debugger_thread_;         // slot
    Fixnum* thread_id_;               // slot
    Randomizer* randomizer_;          // slot
    LookupTable* locals_;             // slot
    Object* group_;                   // slot
    Object* value_;                   // slot
    Exception* exception_;            // slot
    Object* critical_;                // slot
    Fixnum* priority_;                // slot
    Fixnum* pid_;                     // slot
    Object* initialized_;             // slot

    utilities::thread::SpinLock init_lock_;
    utilities::thread::Mutex join_lock_;
    utilities::thread::Condition join_cond_;

    /// The VM state for this thread and this thread alone
    VM* vm_;

    typedef Object* (*ThreadFunction)(STATE);

    ThreadFunction function_;

  public:
    const static object_type type = ThreadType;

    static void bootstrap(STATE);
    static void initialize(STATE, Thread* obj) {
      obj->args_ = nil<Array>();
      obj->block_ = cNil;
      obj->alive_ = cTrue;
      obj->sleep_ = cFalse;
      obj->control_channel_ = nil<Channel>();
      obj->recursive_objects(state, LookupTable::create(state));
      obj->debugger_thread_ = nil<Thread>();
      obj->thread_id_ = nil<Fixnum>();
      obj->randomizer_ = nil<Randomizer>();
      obj->locals(state, LookupTable::create(state));
      obj->group_ = cNil;
      obj->value_ = cNil;
      obj->exception_ = nil<Exception>();
      obj->critical_ = cFalse;
      obj->priority_ = Fixnum::from(0);
      obj->pid_ = Fixnum::from(0);
      obj->initialized_ = cFalse;
      obj->init_lock_.init();
      obj->join_lock_.init();
      obj->join_cond_.init();
      obj->vm_ = 0;
    }

  public:
    attr_accessor(args, Array);
    attr_accessor(block, Object);
    attr_accessor(alive, Object);
    attr_accessor(sleep, Object);
    attr_accessor(control_channel, Channel);
    attr_accessor(recursive_objects, LookupTable);
    attr_accessor(debugger_thread, Thread);
    attr_accessor(thread_id, Fixnum);
    attr_accessor(randomizer, Randomizer);
    attr_accessor(locals, LookupTable);
    attr_accessor(group, Object);
    attr_accessor(value, Object);
    attr_accessor(exception, Exception);
    attr_accessor(critical, Object);
    attr_accessor(priority, Fixnum);
    attr_accessor(pid, Fixnum);
    attr_accessor(initialized, Object);

    VM* vm() const {
      return vm_;
    }

  public:

    // Rubinius.primitive :thread_s_new
    static Thread* s_new(STATE, Object* self, Array* args, Object* block);

    // Rubinius.primitive :thread_s_start
    static Thread* s_start(STATE, Object* self, Array* args, Object* block);

    /**
     *  Returns the Thread object for the state.
     *
     *  This is the currently executing Thread.
     */
    // Rubinius.primitive+ :thread_current
    static Thread* current(STATE);

    /**
     *  Attempt to schedule some other Thread.
     */
    // Rubinius.primitive+ :thread_pass
    static Object* pass(STATE);

    /**
     *   List all live threads.
     */
    // Rubinius.primitive :thread_list
    static Array* list(STATE);

  public:   /* Instance primitives */

    void fork(STATE);

    /**
     *  Retrieve the priority set for this Thread.
     *
     *  The value is numeric, higher being more important
     *  but otherwise *potentially* platform-specific for
     *  any other connotations.
     */
    // Rubinius.primitive+ :thread_get_priority
    Object* get_priority(STATE);

    /**
     *  Process an exception raised for this Thread.
     */
    // Rubinius.primitive :thread_raise
    Object* raise(STATE, Exception* exc);

    /**
     *  Kill this Thread.
     */
    // Rubinius.primitive :thread_kill
    Object* kill(STATE);

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

    // Rubinius.primitive :thread_mri_backtrace
    Array* mri_backtrace(STATE);

    // Rubinius.primitive :thread_join
    Thread* join(STATE, Object* timeout);

    // This method must only be called after fork() with only one active
    // thread.
    void unlock_after_fork(STATE);

    /**
     * Retrieve a value store in the thread locals.
     * This is done in a primitive because it also has
     * to consider any running fibers.
     */
    // Rubinius.primitive+ :thread_locals_aref
    Object* locals_aref(STATE, Symbol* key);

    /**
     * Store a value in the thread locals.
     * This is done in a primitive because it also has
     * to consider any running fibers.
     */
    // Rubinius.primitive :thread_locals_store
    Object* locals_store(STATE, Symbol* key, Object* value);

    /**
     * Remove a value from the thread locals.
     * This is done in a primitive because it also has
     * to consider any running fibers.
     */
    // Rubinius.primitive :thread_locals_remove
    Object* locals_remove(STATE, Symbol* key);

    /**
     * Retrieve the keys for all thread locals.
     * This is done in a primitive because it also has
     * to consider any running fibers.
     */
    // Rubinius.primitive :thread_locals_keys
    Array* locals_keys(STATE);

    /**
     * Check whether a given key has a value store in the thread locals.
     * This is done in a primitive because it also has
     * to consider any running fibers.
     */
    // Rubinius.primitive+ :thread_locals_has_key
    Object* locals_has_key(STATE, Symbol* key);

    void init_lock();
    void stopped();

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
    static Thread* create(STATE, VM* vm);
    static Thread* create(STATE, VM* vm, ThreadFunction function);
    static Thread* create(STATE, Object* self, ThreadFunction function);
    static Thread* create(STATE, Object* self, VM* vm, ThreadFunction function);
    static Thread* create(STATE, Class* klass, VM* vm);

    static void finalize(STATE, Thread* thread);
    void finalize_instance(STATE);

    int start_thread(STATE, void* (*function)(void*));
    static void* run(void*);

    static Object* main_thread(STATE);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}


#endif
