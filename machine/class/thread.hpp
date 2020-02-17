#ifndef RBX_BUILTIN_THREAD_HPP
#define RBX_BUILTIN_THREAD_HPP

#include "object_utils.hpp"

#include "class/channel.hpp"
#include "class/exception.hpp"
#include "class/fiber.hpp"
#include "class/fixnum.hpp"
#include "class/lookup_table.hpp"
#include "class/object.hpp"
#include "class/randomizer.hpp"

#include <list>

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
  public:
    const static object_type type = ThreadType;

    attr_accessor(args, Array);
    attr_accessor(block, Object);
    attr_accessor(recursive_objects, LookupTable);
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
    attr_accessor(stack_size, Fixnum);
    attr_accessor(source, String);
    attr_accessor(fiber, Fiber);
    attr_accessor(current_fiber, Fiber);
    attr_accessor(fiber_value, Object);

  private:
    attr_field(thread_state, ThreadState*);

    typedef Object* (*ThreadFunction)(STATE);

    attr_field(function, ThreadFunction);

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, Thread* obj) {
      obj->args(nil<Array>());
      obj->block(cNil);
      obj->recursive_objects(state, LookupTable::create(state));
      obj->thread_id(nil<Fixnum>());
      obj->randomizer(nil<Randomizer>());
      obj->locals(state, LookupTable::create(state));
      obj->group(cNil);
      obj->value(cNil);
      obj->exception(nil<Exception>());
      obj->critical(cFalse);
      obj->priority(Fixnum::from(0));
      obj->pid(Fixnum::from(0));
      obj->initialized(cFalse);
      obj->stack_size(Fixnum::from(state->configuration()->machine_thread_stack_size.value));
      obj->source(nil<String>());
      obj->fiber(nil<Fiber>());
      obj->current_fiber(nil<Fiber>());
      obj->fiber_value(nil<Object>());
      obj->thread_state(nullptr);
    }

  public:

    // Rubinius.primitive :thread_s_new
    static Thread* s_new(STATE, Object* self, Array* args, Object* kw, Object* block);

    // Rubinius.primitive :thread_s_start
    static Thread* s_start(STATE, Object* self, Array* args, Object* kw, Object* block);

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

    // Rubinius.primitive :thread_count
    static Fixnum* count(STATE);

  public:   /* Instance primitives */

    void fork(STATE);

    // Rubinius.primitive :thread_name
    Object* name(STATE);

    // Rubinius.primitive :thread_set_name
    Object* set_name(STATE, String* name);

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

    // Rubinius.primitive :thread_suspend
    Object* suspend(STATE, Object* duration);

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

    // Rubinius.primitive :thread_fiber_list
    Array* fiber_list(STATE);

    // Rubinius.primitive :thread_fiber_variable_get
    Object* fiber_variable_get(STATE, Symbol* key);

    // Rubinius.primitive :thread_fiber_variable_set
    Object* fiber_variable_set(STATE, Symbol* key, Object* value);

    // Rubinius.primitive :thread_fiber_variable_key_p
    Object* fiber_variable_key_p(STATE, Symbol* key);

    // Rubinius.primitive :thread_fiber_variables
    Array* fiber_variables(STATE);

    // Rubinius.primitive :thread_variable_get
    Object* variable_get(STATE, Symbol* key);

    // Rubinius.primitive :thread_variable_set
    Object* variable_set(STATE, Symbol* key, Object* value);

    // Rubinius.primitive :thread_variable_key_p
    Object* variable_key_p(STATE, Symbol* key);

    // Rubinius.primitive :thread_variables
    Array* variables(STATE);

    // Rubinius.primitive :thread_status
    Object* status(STATE);

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
    static Thread* create(STATE, ThreadState* thread_state);
    static Thread* create(STATE, ThreadState* thread_state, ThreadFunction function);
    static Thread* create(STATE, Object* self, ThreadFunction function);
    static Thread* create(STATE, Object* self, ThreadState* thread_state, ThreadFunction function);
    static Thread* create(STATE, Class* klass, ThreadState* thread_state);

    static void stop(STATE, ThreadState* thread);
    static void finalize(STATE, Thread* thread);

    int start_thread(STATE, void* (*function)(void*));
    static void* run(void*);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}


#endif
