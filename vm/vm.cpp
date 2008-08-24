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

#include "config.hpp"

#include <iostream>

// Reset macros since we're inside state
#undef G
#undef GO
#define G(whatever) globals.whatever.get()
#define GO(whatever) globals.whatever

namespace rubinius {
  VM::VM(size_t bytes) : probe(NULL), wait_events(false) {
    config.compile_up_front = false;
    context_cache = NULL;

    user_config = new ConfigParser();

    om = new ObjectMemory(bytes);

    MethodContext::initialize_cache(this);
    TypeInfo::init(this);

    bootstrap_ontology();

    events = new event::Loop(EVFLAG_FORKCHECK);
    global_cache = new GlobalCache;

    VMLLVMMethod::init("vm/instructions.bc");
    boot_threads();
  }

  VM::~VM() {
    delete om;
    delete events;
    delete global_cache;
    llvm_cleanup();
  }

  void VM::boot_threads() {
    Thread* thr = Thread::create(this);
    thr->boot_task(this);

    activate_thread(thr);
  }

  OBJECT VM::new_object(Class *cls) {
    return om->new_object(cls, cls->instance_fields->to_native());
  }

  Class* VM::new_basic_class(Class* sup, size_t fields) {
    Class *cls = (Class*)om->new_object(G(klass), Class::fields);
    cls->instance_fields = Fixnum::from(fields);
    if(sup->nil_p()) {
      cls->instance_type = Fixnum::from(ObjectType);
    } else {
      cls->instance_type = sup->instance_type; // HACK test that this is always true
    }
    cls->superclass = sup;

    return cls;
  }

  Class* VM::new_class(const char* name) {
    return new_class(name, G(object), G(object)->instance_fields->to_native(),
        G(object));
  }

  Class* VM::new_class(const char* name, Class* super_class) {
    return new_class(name, super_class, G(object)->instance_fields->to_native(),
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
    return new_class(name, G(object), G(object)->instance_fields->to_native(), under);
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
    return om->new_object_bytes(cls, bytes);
  }

  void type_assert(OBJECT obj, object_type type, const char* reason) {
    if(obj->reference_p() && obj->obj_type != type) {
      TypeError::raise(type, obj, reason);
    } else if(type == FixnumType && !obj->fixnum_p()) {
      TypeError::raise(type, obj, reason);
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

  void VM::run_best_thread() {
    Thread* next = NULL;

    events->poll();

    for(size_t i = globals.scheduled_threads->field_count - 1; i > 0; i--) {
      List* lst = as<List>(globals.scheduled_threads->at(i));
      if(lst->empty_p()) continue;
      next = as<Thread>(lst->shift(this));
      break;
    }

    if(!next) {
      if(events->num_of_events() == 0) {
        throw DeadLock("no runnable threads, present or future.");
      }

      wait_events = true;
      return;
    }

    activate_thread(next);
  }

  void VM::return_value(OBJECT val) {
    globals.current_task->push(val);
  }

  void VM::queue_thread(Thread* thread) {
    List* lst = as<List>(globals.scheduled_threads->at(thread->priority->to_native()));
    lst->append(this, thread);
  }

  void VM::activate_thread(Thread* thread) {
    globals.current_thread.set(thread);
    globals.current_task.set(thread->task);
  }

  OBJECT VM::current_block() {
    return globals.current_task->active->block;
  }

  void VM::raise_from_errno(const char* msg) {
    // TODO: implement me
  }

  void VM::raise_exception(Exception* exc) {
    // TODO: implement me
  }

  void VM::inspect(OBJECT obj) {
    if(obj->symbol_p()) {
      String* str = as<Symbol>(obj)->to_str(this);
      std::cout << "<Symbol :" << str->byte_address() << ">" << std::endl;
    } else if(obj->fixnum_p()) {
      std::cout << "<Fixnum " << as<Fixnum>(obj)->to_native() << ">" << std::endl;
    } else {
      std::cout << "<Object: " << (void*)obj << ">" << std::endl;
    }
  }

  void VM::set_const(const char* name, OBJECT val) {
    globals.object->set_const(this, (char*)name, val);
  }

  void VM::set_const(Module* mod, const char* name, OBJECT val) {
    mod->set_const(this, (char*)name, val);
  }

  void VM::print_backtrace() {
    MethodContext* ctx = globals.current_task.get()->active;

    while(!ctx->nil_p()) {
      std::cout << (void*)ctx << ": ";
      if(kind_of<BlockContext>(ctx)) {
        std::cout << "__block__";
      } else {
        // HACK reports Object#[] instead of Hash::[], etc
        std::cout << ctx->module->name->to_str(this)->byte_address() << "#";

        SYMBOL name = try_as<Symbol>(ctx->name);
        if(name) {
          std::cout << name->to_str(this)->byte_address();
        } else {
          std::cout << ctx->cm->name->to_str(this)->byte_address();
        }
      }

      std::cout << ":" << ctx->line() << " in " << ctx->cm->file->to_str(this)->byte_address();

      std::cout << "\n";
      ctx = ctx->sender;
    }
  }

  Task* VM::new_task() {
    Task* task = Task::create(this);
    globals.current_task.set(task);
    return task;
  }

  /* For debugging. */
  extern "C" {
    void __printbt__(STATE) {
      state->print_backtrace();
    }
  }
};
