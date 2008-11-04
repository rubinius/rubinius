#include "vm.hpp"
#include "objectmemory.hpp"
#include "event.hpp"
#include "global_cache.hpp"
#include "llvm.hpp"

#include "vm/object_utils.hpp"

#include "builtin/class.hpp"
#include "builtin/contexts.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/list.hpp"
#include "builtin/symbol.hpp"
#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/string.hpp"
#include "builtin/taskprobe.hpp"

#include "config.hpp"

#include <iostream>
#include <signal.h>

// Reset macros since we're inside state
#undef G
#undef GO
#define G(whatever) globals.whatever.get()
#define GO(whatever) globals.whatever

namespace rubinius {
  VM::VM(size_t bytes) : current_mark(NULL), reuse_llvm(true) {
    config.compile_up_front = false;

    VM::register_state(this);

    user_config = new ConfigParser();

    om = new ObjectMemory(this, bytes);
    probe.set(Qnil, &globals.roots);

    MethodContext::initialize_cache(this);
    TypeInfo::init(this);

    bootstrap_ontology();

    /* @todo Using a single default loop, revisit when many loops.
     * @todo This needs to be handled through the environment.
     * (disabled epoll backend as it frequently caused hangs on epoll_wait)
     */
    signal_events = new event::Loop(EVFLAG_FORKCHECK | EVBACKEND_SELECT | EVBACKEND_POLL);
    events = signal_events;

    signal_events->start(new event::Child::Event(this));

    global_cache = new GlobalCache;

#ifdef ENABLE_LLVM
    VMLLVMMethod::init("vm/instructions.bc");
#endif
    boot_threads();

    // Force these back to false because creating the default Thread
    // sets them to true.
    interrupts.use_preempt = false;
    interrupts.enable_preempt = false;
  }

  VM::~VM() {
    delete om;

    delete signal_events;

    delete global_cache;
#ifdef ENABLE_LLVM
    if(!reuse_llvm) llvm_cleanup();
#endif
  }

  // HACK so not thread safe or anything!
  static VM* __state = NULL;

  VM* VM::current_state() {
    return __state;
  }

  void VM::register_state(VM *vm) {
    __state = vm;
  }

  void VM::boot_threads() {
    Thread* thread = Thread::create(this);

    thread->sleep(this, Qfalse);
    globals.current_thread.set(thread);
    globals.current_task.set(thread->task());
  }

  Object* VM::new_object(Class *cls) {
    return om->new_object(cls, cls->instance_fields()->to_native());
  }

  Class* VM::new_basic_class(Class* sup, size_t fields) {
    Class *cls = (Class*)om->new_object(G(klass), Class::fields);
    cls->instance_fields(this, Fixnum::from(fields));
    if(sup->nil_p()) {
      cls->instance_type(this, Fixnum::from(ObjectType));
    } else {
      cls->instance_type(this, sup->instance_type()); // HACK test that this is always true
    }
    cls->superclass(this, sup);

    return cls;
  }

  Class* VM::new_class(const char* name) {
    return new_class(name, G(object), G(object)->instance_fields()->to_native(),
        G(object));
  }

  Class* VM::new_class(const char* name, Class* super_class) {
    return new_class(name, super_class, G(object)->instance_fields()->to_native(),
        G(object));
  }

  Class* VM::new_class(const char* name, Class* sup, size_t fields) {
    return new_class(name, sup, fields, G(object));
  }

  Class* VM::new_class(const char* name, Class* sup, size_t fields, Module* under) {
    Class* cls = new_basic_class(sup, fields);
    cls->setup(this, name, under);

    // HACK test that we've got the MOP setup properly
    MetaClass::attach(this, cls, sup->metaclass(this));
    return cls;
  }

  Class* VM::new_class_under(const char* name, Module* under) {
    return new_class(name, G(object), G(object)->instance_fields()->to_native(), under);
  }

  Module* VM::new_module(const char* name, Module* under) {
    Module *mod = (Module*)om->new_object(G(module), Module::fields);
    mod->setup(this, name, under);
    return mod;
  }


  Symbol* VM::symbol(const char* str) {
    return symbols.lookup(this, str);
  }

  Symbol* VM::symbol(String* str) {
    return symbols.lookup(this, str);
  }

  Symbol* VM::symbol(std::string str) {
    return symbols.lookup(this, str);
  }

  Object* VM::new_struct(Class* cls, size_t bytes) {
    Object* obj = om->new_object_bytes(cls, bytes);
    obj->ivars(this, Qnil);
    return obj;
  }

  void type_assert(STATE, Object* obj, object_type type, const char* reason) {
    if((obj->reference_p() && obj->obj_type != type)
        || (type == FixnumType && !obj->fixnum_p())) {
      Exception::type_error(state, type, obj, reason);
    }
  }

  void VM::add_type_info(TypeInfo* ti) {
    om->add_type_info(ti);
    ti->state = this;
  }

  TypeInfo* VM::find_type(int type) {
    return om->type_info[type];
  }

  Thread *VM::current_thread() {
    return globals.current_thread.get();
  }

  void VM::run_gc_soon() {
    om->collect_young_now = true;
    om->collect_mature_now = true;
    interrupts.check = true;
  }

  void VM::collect() {
    om->collect_young(globals.roots);
    om->collect_mature(globals.roots);
  }

  void VM::collect_maybe() {
    if(om->collect_young_now) {
      om->collect_young_now = false;
      om->collect_young(globals.roots);
      global_cache->clear();
    }

    if(om->collect_mature_now) {
      om->collect_mature_now = false;
      om->collect_mature(globals.roots);
      global_cache->clear();
    }

    /* Stack Management procedures. Make sure that we don't
     * miss object stored into the stack of a context */
    if(G(current_task)->active()->zone == MatureObjectZone) {
      om->remember_object(G(current_task)->active());
    }

    if(G(current_task)->home()->zone == MatureObjectZone &&
        !G(current_task)->home()->Remember) {
      om->remember_object(G(current_task)->home());
    }
  }

  bool VM::find_and_activate_thread() {
    Tuple* scheduled = globals.scheduled_threads.get();

    for(std::size_t i = scheduled->num_fields() - 1; i > 0; i--) {
      List* list = as<List>(scheduled->at(this, i));

      Thread* thread = try_as<Thread>(list->shift(this));

      while(thread) {
        thread->queued(this, Qfalse);

        /** @todo   Should probably try to prevent dead threads here.. */
        if(thread->alive() == Qfalse) {
          thread = try_as<Thread>(list->shift(this));
          continue;
        }

        if(thread->sleep() == Qtrue) {
          thread = try_as<Thread>(list->shift(this));
          continue;
        }

        activate_thread(thread);
        return true;
      }
    }

    return false;
  }

  void VM::check_events() {
    interrupts.check = true;
    interrupts.check_events = true;
  }

  bool VM::run_best_thread() {
    events->poll();

    if(!find_and_activate_thread()) {
      // It's 1 because the event that looks for SIGCHLD is
      // always registered.
      if(events->num_of_events() == 1) {
        throw DeadLock("no runnable threads, present or future.");
      }

      interrupts.check_events = true;
      return false;
    }
    return true;
  }

  void VM::return_value(Object* val) {
    globals.current_task->push(val);
  }

  void VM::queue_thread(Thread* thread) {
    if(thread->queued() == Qtrue) {
      return;
    }

    List* lst = as<List>(globals.scheduled_threads->at(this,
                         thread->priority()->to_native()));
    lst->append(this, thread);

    thread->queued(this, Qtrue);
  }

  void VM::dequeue_thread(Thread* thread) {
    thread->queued(this, Qfalse);

    Tuple* scheduled = globals.scheduled_threads.get();

    /** @todo  Could it be in more than one somehow? --rue */
    List* list = try_as<List>(scheduled->at(this, thread->priority()->to_native()));
    (void) list->remove(this, thread);

    check_events();
  }

  void VM::activate_thread(Thread* thread) {
    if(thread == globals.current_thread.get()) {
      thread->task(this, globals.current_task.get());
      return;
    }

    /* May have been using Tasks directly. */
    globals.current_thread->task(this, globals.current_task.get());
    queue_thread(globals.current_thread.get());

    thread->sleep(this, Qfalse);
    globals.current_thread.set(thread);

    if(globals.current_task.get() != thread->task()) {
      activate_task(thread->task());
    }
  }

  void VM::activate_task(Task* task) {
    // Don't try and reclaim any contexts, they belong to someone else.
    om->clamp_contexts();

    globals.current_task.set(task);
    interrupts.check = true;
  }

  Object* VM::current_block() {
    return globals.current_task->active()->block();
  }

  void VM::raise_from_errno(const char* msg) {
    // @todo implement me
  }

  void VM::raise_exception(Exception* exc) {
    // @todo implement me
  }

  void VM::set_const(const char* name, Object* val) {
    globals.object->set_const(this, (char*)name, val);
  }

  void VM::set_const(Module* mod, const char* name, Object* val) {
    mod->set_const(this, (char*)name, val);
  }

  void VM::print_backtrace() {
    globals.current_task.get()->print_backtrace();
  }

  Task* VM::new_task() {
    Task* task = Task::create(this);
    activate_task(task);
    return task;
  }

  void VM::run_and_monitor() {
    for(;;) {
      if(interrupts.check_events) {
        interrupts.check_events = false;
        interrupts.enable_preempt = false;

        Thread* current = G(current_thread);
        // The current thread isn't asleep, so we're being preemptive
        if(current->alive() == Qtrue && current->sleep() != Qtrue) {
          // Order is important here. We poll so any threads
          // might get woken up if they need to be.
          events->poll();

          // Only then do we reschedule the current thread if
          // we need to. queue_thread() puts the thread at the end
          // of the list for it's priority, so we shouldn't starve
          // anything this way.
          queue_thread(G(current_thread));
        }

        while(!find_and_activate_thread()) {
          events->run_and_wait();
        }

        interrupts.enable_preempt = interrupts.use_preempt;
      }

      collect_maybe();
      G(current_task)->execute();
    }
  }

  // Trampoline to call scheduler_loop()
  static void* __thread_tramp__(void* arg) {
    VM* state = static_cast<VM*>(arg);
    state->scheduler_loop();
    return NULL;
  }

  // Create the preemption thread and call scheduler_loop() in the new thread
  void VM::setup_preemption() {
    if(pthread_create(&preemption_thread, NULL, __thread_tramp__, this) != 0) {
      std::cout << "Unable to create preemption thread!\n";
    }
  }

  // Runs forever, telling the VM to reschedule threads ever 10 milliseconds
  void VM::scheduler_loop() {
    // First off, we don't want this thread ever receiving a signal.
    sigset_t mask;
    sigfillset(&mask);
    if(pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0) {
      abort();
    }

    struct timespec requested;
    struct timespec actual;

    requested.tv_sec = 0;
    requested.tv_nsec = 10000000; // 10 milliseconds

    for(;;) {
      nanosleep(&requested, &actual);
      if(interrupts.enable_preempt) {
        interrupts.reschedule = true;
        interrupts.check_events = true;
      }
    }
  }

  /* For debugging. */
  extern "C" {
    void __printbt__() {
      VM::current_state()->print_backtrace();
    }
  }
};
