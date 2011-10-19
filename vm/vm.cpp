#include "vm.hpp"
#include "objectmemory.hpp"
#include "global_cache.hpp"
#include "gc/gc.hpp"

#include "vm/object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/array.hpp"
#include "builtin/list.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/symbol.hpp"
#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/string.hpp"
#include "builtin/system.hpp"
#include "builtin/fiber.hpp"
#include "builtin/location.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/channel.hpp"

#include "instruments/tooling.hpp"
#include "instruments/rbxti-internal.hpp"
#include "instruments/timing.hpp"

#include "config_parser.hpp"
#include "config.h"

#include "call_frame.hpp"
#include "signal.hpp"
#include "configuration.hpp"
#include "helpers.hpp"

#include "util/thread.hpp"

#include <iostream>
#include <iomanip>
#include <signal.h>
#ifndef RBX_WINDOWS
#include <sys/resource.h>
#endif

// Reset macros since we're inside state
#undef G
#undef GO
#define G(whatever) globals().whatever.get()
#define GO(whatever) globals().whatever

namespace rubinius {

  unsigned long VM::cStackDepthMax = 655300;

#ifndef RBX_WINDOWS
  /**
   * Maximum amount of stack space to use.
   * getrlimit can report there is 4G of stack (ie, unlimited).  Even when
   * there is unlimited stack, we clamp the max to this value (currently 128M).
   */
  static rlim_t cMaxStack = (1024 * 1024 * 128);
#endif

  VM::VM(uint32_t id, SharedState& shared)
    : ManagedThread(id, shared, ManagedThread::eRuby)
    , saved_call_frame_(0)
    , stack_start_(0)
    , run_signals_(false)
    , thread_step_(false)

    , shared(shared)
    , waiting_channel_(this, (Channel*)Qnil)
    , interrupted_exception_(this, (Exception*)Qnil)
    , interrupt_with_signal_(false)
    , waiting_header_(0)
    , custom_wakeup_(0)
    , custom_wakeup_data_(0)
    , om(shared.om)
    , interrupts(shared.interrupts)
    , check_local_interrupts(false)
    , thread_state_(this)
    , thread(this, nil<Thread>())
    , current_fiber(this, nil<Fiber>())
    , root_fiber(this, nil<Fiber>())
  {
    set_stack_size(cStackDepthMax);

    if(shared.om) {
      young_start_ = shared.om->young_start();
      young_end_ = shared.om->yound_end();
      shared.om->refill_slab(this, local_slab_);
    }

    tooling_env_ = rbxti::create_env(this);
    tooling_ = false;
  }

  void VM::discard(STATE, VM* vm) {
    rbxti::destroy_env(vm->tooling_env_);
    vm->saved_call_frame_ = 0;
    vm->shared.remove_vm(vm);

    delete vm;
  }

  void VM::initialize_as_root() {
    om = new ObjectMemory(this, shared.config);
    shared.om = om;

    young_start_ = shared.om->young_start();
    young_end_ = shared.om->yound_end();

    om->refill_slab(this, local_slab_);

    shared.set_initialized();

    shared.gc_dependent(this);

    TypeInfo::auto_learn_fields(this);

    bootstrap_ontology();

    VMMethod::init(this);

    // Setup the main Thread, which is wrapper of the main native thread
    // when the VM boots.
    thread.set(Thread::create(this, this, G(thread), 0, true), &globals().roots);
    thread->sleep(this, Qfalse);

    VM::set_current(this);
  }

  void VM::initialize_config() {
#ifdef ENABLE_LLVM
    if(!shared.config.jit_disabled) {
      Array* ary = Array::create(this, 3);
      ary->append(this, symbol("usage"));
      if(shared.config.jit_inline_generic) {
        ary->append(this, symbol("inline_generic"));
      }

      if(shared.config.jit_inline_blocks) {
        ary->append(this, symbol("inline_blocks"));
      }
      G(rubinius)->set_const(this, "JIT", ary);
    } else {
      G(rubinius)->set_const(this, "JIT", Qfalse);
    }
#else
    G(rubinius)->set_const(this, "JIT", Qnil);
#endif
  }

  /**
   * Returns the current VM executing on this pthread.
   */
  VM* VM::current() {
    return ManagedThread::current()->as_vm();
  }

  /**
   * Sets this VM instance as the current VM on this pthread.
   */
  void VM::set_current(VM* vm) {
    ManagedThread::set_current(vm);
  }

  Object* VM::new_object_typed(Class* cls, size_t size, object_type type) {

    if(unlikely(size > om->large_object_threshold)) {
      return om->new_object_typed_enduring(this, cls, size, type);
    }

    Object* obj = local_slab().allocate(size).as<Object>();

    if(unlikely(!obj)) {
      if(shared.om->refill_slab(this, local_slab())) {
        obj = local_slab().allocate(size).as<Object>();
      }

      // If refill_slab fails, obj will still be NULL.

      if(!obj) {
        return om->new_object_typed(this, cls, size, type);
      }
    }

    obj->init_header(cls, YoungObjectZone, type);
    obj->clear_fields(size);

    return obj;
  }

  Tuple* VM::new_young_tuple_dirty(size_t fields) {
    size_t bytes = sizeof(Tuple) + (sizeof(Object*) * fields);

    if(unlikely(bytes > om->large_object_threshold)) {
      return 0;
    }

    Tuple* tup = local_slab().allocate(bytes).as<Tuple>();

    if(unlikely(!tup)) {
      if(shared.om->refill_slab(this, local_slab())) {
        tup = local_slab().allocate(bytes).as<Tuple>();
      }

      if(!tup) return 0;
    }

    tup->init_header(G(tuple), YoungObjectZone, Tuple::type);
    tup->full_size_ = bytes;

    return tup;
  }

  Object* VM::new_object_typed_mature(Class* cls, size_t bytes, object_type type) {
    return om->new_object_typed_mature(this, cls, bytes, type);
  }

  Object* VM::new_object_from_type(Class* cls, TypeInfo* ti) {
    return new_object_typed(cls, ti->instance_size, ti->type);
  }

  Class* VM::new_basic_class(Class* sup) {
    Class *cls = om->new_object_enduring<Class>(this, G(klass));
    cls->init(shared.inc_class_count(this));

    if(sup->nil_p()) {
      cls->instance_type(this, Fixnum::from(ObjectType));
      cls->set_type_info(find_type(ObjectType));
    } else {
      cls->instance_type(this, sup->instance_type()); // HACK test that this is always true
      cls->set_type_info(sup->type_info());
    }
    cls->superclass(this, sup);

    return cls;
  }

  Class* VM::new_class(const char* name) {
    return new_class(name, G(object), G(object));
  }

  Class* VM::new_class(const char* name, Class* super_class) {
    return new_class(name, super_class, G(object));
  }

  Class* VM::new_class(const char* name, Class* sup, Module* under) {
    Class* cls = new_basic_class(sup);
    cls->setup(this, name, under);

    // HACK test that we've got the MOP setup properly
    SingletonClass::attach(this, cls, sup->singleton_class(this));
    return cls;
  }

  Class* VM::new_class_under(const char* name, Module* under) {
    return new_class(name, G(object), under);
  }

  Module* VM::new_module(const char* name, Module* under) {
    Module *mod = new_object<Module>(G(module));
    mod->setup(this, name, under);
    return mod;
  }

  Symbol* VM::symbol(const char* str) {
    return shared.symbols.lookup(str, strlen(str));
  }

  Symbol* VM::symbol(std::string str) {
    return shared.symbols.lookup(this, str);
  }

  Symbol* VM::symbol(String* str) {
    return shared.symbols.lookup(this, str);
  }

  void type_assert(STATE, Object* obj, object_type type, const char* reason) {
    if((obj->reference_p() && obj->type_id() != type)
        || (type == FixnumType && !obj->fixnum_p())) {
      Exception::type_error(state, type, obj, reason);
    }
  }

  void VM::raise_stack_error(CallFrame* call_frame) {
    G(stack_error)->locations(this, Location::from_call_stack(this, call_frame));
    thread_state()->raise_exception(G(stack_error));
  }

  void VM::init_stack_size() {
#ifndef RBX_WINDOWS
    struct rlimit rlim;
    if(getrlimit(RLIMIT_STACK, &rlim) == 0) {
      rlim_t space = rlim.rlim_cur/5;

      if(space > 1024*1024) space = 1024*1024;
      rlim_t adjusted = (rlim.rlim_cur - space);

      if(adjusted > cMaxStack) {
        cStackDepthMax = cMaxStack;
      } else {
        cStackDepthMax = adjusted;
      }
    }
#endif
  }

  TypeInfo* VM::find_type(int type) {
    return om->type_info[type];
  }

  Thread *VM::current_thread() {
    return globals().current_thread.get();
  }

  void VM::run_gc_soon() {
    om->collect_young_now = true;
    om->collect_mature_now = true;
    interrupts.set_perform_gc();
  }

  void VM::collect(CallFrame* call_frame) {
    this->set_call_frame(call_frame);
    om->collect(this, call_frame);
  }

  void VM::collect_maybe(CallFrame* call_frame) {
    this->set_call_frame(call_frame);
    om->collect_maybe(this, call_frame);
  }

  void VM::set_const(const char* name, Object* val) {
    globals().object->set_const(this, (char*)name, val);
  }

  void VM::set_const(Module* mod, const char* name, Object* val) {
    mod->set_const(this, (char*)name, val);
  }

  Object* VM::path2class(const char* path) {
    Module* mod = shared.globals.object.get();

    char* copy = strdup(path);
    char* cur = copy;

    for(;;) {
      char* pos = strstr(cur, "::");
      if(pos) *pos = 0;

      Object* obj = mod->get_const(this, symbol(cur));

      if(pos) {
        if(Module* m = try_as<Module>(obj)) {
          mod = m;
        } else {
          free(copy);
          return Qnil;
        }
      } else {
        free(copy);
        return obj;
      }

      cur = pos + 2;
    }
  }

  void VM::print_backtrace() {
    abort();
  }

  void VM::interrupt_with_signal() {
    interrupt_with_signal_ = true;
  }

  bool VM::wakeup(STATE) {
    SYNC(state);

    check_local_interrupts = true;

    // Wakeup any locks hanging around with contention
    om->release_contention(this);

    if(interrupt_with_signal_) {
#ifdef RBX_WINDOWS
      // TODO: wake up the thread
#else
      pthread_kill(os_thread_, SIGVTALRM);
#endif
      return true;
    } else if(InflatedHeader* ih = waiting_header_) {
      // We shouldn't hold the VM lock and the IH lock at the same time,
      // other threads can grab them and deadlock.
      UNSYNC;
      ih->wakeup();
      return true;
    } else {
      Channel* chan = waiting_channel_.get();

      if(!chan->nil_p()) {
        UNSYNC;
        chan->send(state, Qnil);
        return true;
      } else if(custom_wakeup_) {
        UNSYNC;
        (*custom_wakeup_)(custom_wakeup_data_);
        return true;
      }

      return false;
    }
  }

  void VM::clear_waiter() {
    SYNC(0);
    interrupt_with_signal_ = false;
    waiting_channel_.set((Channel*)Qnil);
    waiting_header_ = 0;
    custom_wakeup_ = 0;
    custom_wakeup_data_ = 0;
  }

  void VM::wait_on_channel(Channel* chan) {
    SYNC(0);
    thread->sleep(this, Qtrue);
    waiting_channel_.set(chan);
  }

  void VM::wait_on_inflated_lock(InflatedHeader* ih) {
    SYNC(0);
    waiting_header_ = ih;
  }

  void VM::wait_on_custom_function(void (*func)(void*), void* data) {
    SYNC(0);
    custom_wakeup_ = func;
    custom_wakeup_data_ = data;
  }

  bool VM::waiting_p() {
    return interrupt_with_signal_ || !waiting_channel_->nil_p();
  }

  void VM::set_sleeping() {
    thread->sleep(this, Qtrue);
  }

  void VM::clear_sleeping() {
    thread->sleep(this, Qfalse);
  }

  bool VM::process_async(CallFrame* call_frame) {
    check_local_interrupts = false;

    if(run_signals_) {
      if(!shared.signal_handler()->deliver_signals(this, call_frame)) {
        return false;
      }
    }

    Exception* exc = interrupted_exception_.get();
    if(!exc->nil_p()) {
      interrupted_exception_.set((Exception*)Qnil);

      // Only write the locations if there are none.
      if(exc->locations()->nil_p() || exc->locations()->size() == 0) {
        exc->locations(this, Location::from_call_stack(this, call_frame));
      }

      thread_state_.raise_exception(exc);
      return false;
    }

    return true;
  }

  void VM::register_raise(STATE, Exception* exc) {
    SYNC(state);
    interrupted_exception_.set(exc);
    check_local_interrupts = true;
    get_attention();
  }

  void VM::check_exception(CallFrame* call_frame) {
    if(thread_state()->raise_reason() == cNone) {
      std::cout << "Exception propagating, but none registered!\n";
      call_frame->print_backtrace(this);
      rubinius::abort();
    }
  }

  bool VM::check_interrupts(CallFrame* call_frame, void* end) {
    // First, we might be here because someone reset the stack_limit_ so that
    // we'd fall into here to check interrupts even if the stack is fine,
    //
    // So fix up the stack_limit_ if thats the case first.

    // If this is true, stack_limit_ was just changed to get our attention, reset
    // it now.
    if(stack_limit_ == stack_start_) {
      reset_stack_limit();
    } else {
      if(!check_stack(call_frame, end)) return false;
    }

    if(unlikely(check_local_interrupts)) {
      if(!process_async(call_frame)) return false;
    }

    // If the current thread is trying to step, debugger wise, then assist!
    if(thread_step()) {
      clear_thread_step();
      if(!Helpers::yield_debugger(this, call_frame, Qnil)) return false;
    }

    return true;
  }

  void VM::set_current_fiber(Fiber* fib) {
    set_stack_start(fib->stack());
    set_stack_size(fib->stack_size());
    current_fiber.set(fib);
  }

  GCIndependent::GCIndependent(NativeMethodEnvironment* env)
    : vm_(env->state())
  {
    vm_->set_call_frame(env->current_call_frame());
    vm_->shared.gc_independent(vm_);
  };
};
