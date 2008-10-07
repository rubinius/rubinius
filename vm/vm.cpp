#include "vm.hpp"
#include "objectmemory.hpp"
#include "event.hpp"
#include "global_cache.hpp"
#include "llvm.hpp"
#include "builtin/class.hpp"
#include "builtin/contexts.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/list.hpp"
#include "builtin/symbol.hpp"
#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/string.hpp"
#include "builtin/taskprobe.hpp"

#include "context_cache.hpp"
#include "config.hpp"

#include <iostream>

// Reset macros since we're inside state
#undef G
#undef GO
#define G(whatever) globals.whatever.get()
#define GO(whatever) globals.whatever

namespace rubinius {
  VM::VM(size_t bytes) : wait_events(false), reuse_llvm(true) {
    config.compile_up_front = false;
    context_cache = NULL;

    VM::register_state(this);

    user_config = new ConfigParser();

    om = new ObjectMemory(this, bytes);
    probe.set(Qnil, &globals.roots);

    MethodContext::initialize_cache(this);
    TypeInfo::init(this);

    bootstrap_ontology();

    /* TODO: Using a single default loop, revisit when many loops.
     * TODO: This needs to be handled through the environment.
     */
    signal_events = new event::Loop(EVFLAG_FORKCHECK);
    events = signal_events;

    signal_events->start(new event::Child::Event(this));

    global_cache = new GlobalCache;

    VMLLVMMethod::init("vm/instructions.bc");
    boot_threads();
  }

  VM::~VM() {
    delete om;
    delete signal_events;
    delete global_cache;
    if(!reuse_llvm) llvm_cleanup();
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
    Thread* thr = Thread::create(this);

    activate_thread(thr);
  }

  OBJECT VM::new_object(Class *cls) {
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


  SYMBOL VM::symbol(const char* str) {
    return symbols.lookup(this, str);
  }

  SYMBOL VM::symbol(String* str) {
    return symbols.lookup(this, str);
  }

  SYMBOL VM::symbol(std::string str) {
    return symbols.lookup(this, str);
  }

  OBJECT VM::new_struct(Class* cls, size_t bytes) {
    Object* obj = om->new_object_bytes(cls, bytes);
    obj->ivars(this, Qnil);
    return obj;
  }

  void type_assert(STATE, OBJECT obj, object_type type, const char* reason) {
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

  void VM::collect() {
    om->collect_young(globals.roots);
    om->collect_mature(globals.roots);
  }

  bool VM::find_and_activate_thread() {
    for(size_t i = globals.scheduled_threads->num_fields() - 1; i > 0; i--) {
      List* lst = as<List>(globals.scheduled_threads->at(this, i));
      if(lst->empty_p()) continue;
      activate_thread(as<Thread>(lst->shift(this)));
      return true;
    }

    return false;
  }

  bool VM::run_best_thread() {
    events->poll();

    if(!find_and_activate_thread()) {
      if(events->num_of_events() == 0) {
        throw DeadLock("no runnable threads, present or future.");
      }

      wait_events = true;
      return false;
    }
    return true;
  }

  void VM::return_value(OBJECT val) {
    globals.current_task->push(val);
  }

  void VM::queue_thread(Thread* thread) {
    List* lst = as<List>(globals.scheduled_threads->at(this,
          thread->priority()->to_native()));
    lst->append(this, thread);

    thread->woken(this);
  }

  void VM::activate_thread(Thread* thread) {
    globals.current_thread.set(thread);
    if(globals.current_task.get() != thread->task()) {
      activate_task(thread->task());
    }

    thread->woken(this);
  }

  void VM::activate_task(Task* task) {
    // Don't try and reclaim any contexts, they belong to someone else.
    context_cache->reclaim = 0;

    globals.current_task.set(task);
    interrupts.check = true;
    interrupts.switch_task = true;
  }

  OBJECT VM::current_block() {
    return globals.current_task->active()->block();
  }

  void VM::raise_from_errno(const char* msg) {
    // TODO: implement me
  }

  void VM::raise_exception(Exception* exc) {
    // TODO: implement me
  }

  void VM::set_const(const char* name, OBJECT val) {
    globals.object->set_const(this, (char*)name, val);
  }

  void VM::set_const(Module* mod, const char* name, OBJECT val) {
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
      while(wait_events) {
        wait_events = false;
        events->run_and_wait();
        wait_events = !find_and_activate_thread();
      }

      G(current_task)->check_interrupts();
      G(current_task)->execute();
    }
  }

  /* For debugging. */
  extern "C" {
    void __printbt__() {
      VM::current_state()->print_backtrace();
    }
  }
};
