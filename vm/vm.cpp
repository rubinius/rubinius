#include "vm.hpp"
#include "objectmemory.hpp"
#include "objects.hpp"
#include "event.hpp"
#include "global_cache.hpp"

namespace rubinius {
  VM::VM(size_t bytes) {
    om = new ObjectMemory(bytes);
    bootstrap_ontology();

    events = new event::Loop(EVFLAG_FORKCHECK);
    global_cache = new GlobalCache;
  }

  VM::~VM() {
    delete om;
    delete events;
    delete global_cache;
  }

  OBJECT VM::new_object(Class *cls) {
    return om->new_object(cls, cls->instance_fields->n2i());
  }

  Symbol* VM::symbol(const char* str, size_t size) {
    return (Symbol*)globals.symbols->lookup(this, str, size);
  }

  OBJECT VM::new_struct(Class* cls, size_t bytes) {
    return om->new_object_bytes(cls, bytes);
  }

  void type_assert(OBJECT obj, object_type type, char* reason) {
    if(obj->reference_p() && obj->obj_type != type) {
      throw new TypeError(type, obj, reason);
    } else if(type == FixnumType && !obj->fixnum_p()) {
      throw new TypeError(type, obj, reason);
    }
  }

  void VM::add_type_info(TypeInfo* ti) {
    om->add_type_info(ti);
    ti->state = this;
  }

  OBJECT VM::current_thread() {
    return Qnil;
  }

  void VM::collect() {
    om->collect_young(globals.roots);
    om->collect_mature(globals.roots);
  }

  void VM::run_best_thread() {
    throw new DeadLock("no runnable threads, present or future.");
  }

  void VM::return_value(OBJECT val) {
    globals.current_task->push(val);
  }

  void VM::queue_thread(Thread* thread) {
    List* lst = as<List>(globals.scheduled_threads->at(thread->priority->n2i()));
    lst->append(this, thread);
  }

  void VM::activate_thread(Thread* thread) {
    globals.current_thread.set(thread);
    globals.current_task.set(thread->task);
  }
};
