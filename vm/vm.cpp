#include "vm.hpp"
#include "objectmemory.hpp"
#include "global_cache.hpp"
#include "gc/gc.hpp"

#include "vm/object_utils.hpp"

#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/list.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/symbol.hpp"
#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/string.hpp"
#include "builtin/system.hpp"
#include "builtin/fiber.hpp"
#include "builtin/location.hpp"

#include "instruments/tooling.hpp"
#include "instruments/rbxti-internal.hpp"
#include "instruments/timing.hpp"

#include "config_parser.hpp"
#include "config.h"

#include "native_thread.hpp"
#include "call_frame.hpp"
#include "signal.hpp"
#include "configuration.hpp"
#include "helpers.hpp"

#include "util/thread.hpp"

#include <iostream>
#include <iomanip>
#include <signal.h>
#include <sys/resource.h>

// Reset macros since we're inside state
#undef G
#undef GO
#define G(whatever) globals().whatever.get()
#define GO(whatever) globals().whatever

namespace rubinius {

  bool GlobalLock::debug_locking = false;
  unsigned long VM::cStackDepthMax = 655300;

  // getrlimit can report there is 4G of stack (ie, unlimited).
  // Even when there is unlimited stack, we clamp the max to
  // this value (currently 128M)
  static rlim_t cMaxStack = (1024 * 1024 * 128);

  VM::VM(SharedState& shared)
    : ManagedThread(shared, ManagedThread::eRuby)
    , saved_call_frame_(0)
    , stack_start_(0)
    , run_signals_(false)
    , thread_step_(false)

    , shared(shared)
    , waiter_(NULL)
    , interrupt_with_signal_(false)
    , om(shared.om)
    , interrupts(shared.interrupts)
    , check_local_interrupts(false)
    , thread_state_(this)
    , thread(this, nil<Thread>())
    , current_fiber(this, nil<Fiber>())
    , root_fiber(this, nil<Fiber>())
  {
    set_stack_size(cStackDepthMax);
    os_thread_ = pthread_self(); // initial value

    if(shared.om) {
      young_start_ = shared.om->young_start();
      young_end_ = shared.om->yound_end();
      shared.om->refill_slab(local_slab_);
    }

    tooling_env_ = rbxti::create_env(this);
    tooling_ = false;
  }

  void VM::discard(VM* vm) {
    vm->saved_call_frame_ = 0;
    vm->shared.remove_vm(vm);
    delete vm;
  }

  void VM::initialize() {
    VM::register_state(this);

    om = new ObjectMemory(this, shared.config);
    shared.om = om;

    young_start_ = shared.om->young_start();
    young_end_ = shared.om->yound_end();

    om->refill_slab(local_slab_);

    shared.set_initialized();

    // This seems like we should do this in VM(), ie, for every VM and
    // therefore every Thread object in the process. But in fact, because
    // we're using the GIL atm, we only do it once. When the GIL goes
    // away, this needs to be moved to VM().

    shared.gc_dependent();
  }

  void VM::boot() {
    TypeInfo::auto_learn_fields(this);

    bootstrap_ontology();

    VMMethod::init(this);

    // Setup the main Thread, which is a reflect of the pthread_self()
    // when the VM boots.
    boot_threads();

    GlobalLock::debug_locking = shared.config.gil_debug;
  }

  void VM::initialize_config() {
#ifdef USE_DYNAMIC_INTERPRETER
    if(shared.config.dynamic_interpreter_enabled) {
      G(rubinius)->set_const(this, "INTERPRETER", symbol("dynamic"));
    } else {
      G(rubinius)->set_const(this, "INTERPRETER", symbol("static"));
    }
#else
    G(rubinius)->set_const(this, "INTERPRETER", symbol("static"));
#endif

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

  // HACK so not thread safe or anything!
  static VM* __state = NULL;

  VM* VM::current_state() {
    return __state;
  }

  void VM::register_state(VM *vm) {
    __state = vm;
  }

  thread::ThreadData<VM*> _current_vm;

  VM* VM::current() {
    return _current_vm.get();
  }

  void VM::set_current(VM* vm) {
    vm->os_thread_ = pthread_self();
    _current_vm.set(vm);
  }

  void VM::boot_threads() {
    thread.set(Thread::create(this, this, G(thread), pthread_self()), &globals().roots);
    thread->sleep(this, Qfalse);

    VM::set_current(this);
  }

  Object* VM::new_object_typed(Class* cls, size_t size, object_type type) {
    Object* obj = reinterpret_cast<Object*>(local_slab().allocate(size));

    if(unlikely(!obj)) {
      if(shared.om->refill_slab(local_slab())) {
        obj = reinterpret_cast<Object*>(local_slab().allocate(size));
      }

      // If refill_slab fails, obj will still be NULL.

      if(!obj) {
        return om->new_object_typed(cls, size, type);
      }
    }

    obj->init_header(cls, YoungObjectZone, type);
    obj->clear_fields(size);

    return obj;
  }

  Object* VM::new_object_typed_mature(Class* cls, size_t bytes, object_type type) {
    return om->new_object_typed_mature(cls, bytes, type);
  }

  Object* VM::new_object_from_type(Class* cls, TypeInfo* ti) {
    return new_object_typed(cls, ti->instance_size, ti->type);
  }

  Class* VM::new_basic_class(Class* sup) {
    Class *cls = om->new_object_enduring<Class>(G(klass));
    cls->init(shared.inc_class_count());

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

    // Don't go any further unless we're allowed to GC.
    if(!om->can_gc()) return;

    // Stops all other threads, so we're only here by ourselves.
    StopTheWorld guard(this);

    GCData gc_data(this);

    om->collect_young(gc_data);
    om->collect_mature(gc_data);

    om->run_finalizers(this, call_frame);
  }

  void VM::collect_maybe(CallFrame* call_frame) {
    this->set_call_frame(call_frame);

    // Don't go any further unless we're allowed to GC.
    if(!om->can_gc()) return;

    // Stops all other threads, so we're only here by ourselves.
    StopTheWorld guard(this);

    GCData gc_data(this);

    uint64_t start_time = 0;

    if(om->collect_young_now) {
      if(shared.config.gc_show) {
        start_time = get_current_time();
      }

      YoungCollectStats stats;

#ifdef RBX_PROFILER
      if(unlikely(tooling())) {
        tooling::GCEntry method(this, tooling::GCYoung);
        om->collect_young(gc_data, &stats);
      } else {
        om->collect_young(gc_data, &stats);
      }
#else
      om->collect_young(gc_data, &stats);
#endif

      if(shared.config.gc_show) {
        uint64_t fin_time = get_current_time();
        int diff = (fin_time - start_time) / 1000000;

        fprintf(stderr, "[GC %0.1f%% %d/%d %d %2dms]\n",
                  stats.percentage_used,
                  stats.promoted_objects,
                  stats.excess_objects,
                  stats.lifetime,
                  diff);
      }
    }

    if(om->collect_mature_now) {
      int before_kb = 0;

      if(shared.config.gc_show) {
        start_time = get_current_time();
        before_kb = om->mature_bytes_allocated() / 1024;
      }

#ifdef RBX_PROFILER
      if(unlikely(tooling())) {
        tooling::GCEntry method(this, tooling::GCMature);
        om->collect_mature(gc_data);
      } else {
        om->collect_mature(gc_data);
      }
#else
      om->collect_mature(gc_data);
#endif

      if(shared.config.gc_show) {
        uint64_t fin_time = get_current_time();
        int diff = (fin_time - start_time) / 1000000;
        int kb = om->mature_bytes_allocated() / 1024;
        fprintf(stderr, "[Full GC %dkB => %dkB %2dms]\n",
            before_kb,
            kb,
            diff);
      }

    }

    // Count the finalizers toward running the mature gc. Not great,
    // but better than not seeing the time at all.
#ifdef RBX_PROFILER
      if(unlikely(tooling())) {
        tooling::GCEntry method(this, tooling::GCFinalizer);
        om->run_finalizers(this, call_frame);
      } else {
        om->run_finalizers(this, call_frame);
      }
#else
      om->run_finalizers(this, call_frame);
#endif
  }

  void VM::set_const(const char* name, Object* val) {
    globals().object->set_const(this, (char*)name, val);
  }

  void VM::set_const(Module* mod, const char* name, Object* val) {
    mod->set_const(this, (char*)name, val);
  }

  void VM::print_backtrace() {
    abort();
  }

  void VM::install_waiter(Waiter& waiter) {
    waiter_ = &waiter;
  }

  void VM::interrupt_with_signal() {
    interrupt_with_signal_ = true;
  }

  bool VM::wakeup() {
    if(interrupt_with_signal_) {
      pthread_kill(os_thread_, SIGVTALRM);
      return true;
    } else {
      // Use a local here because waiter_ can get reset to NULL by another thread
      // We can't use a mutex here because this is called from inside a
      // signal handler.
      if(Waiter* w = waiter_) {
        w->run();
        return true;
      }

      return false;
    }
  }

  void VM::clear_waiter() {
    interrupt_with_signal_ = false;
    waiter_ = NULL;
  }

  bool VM::process_async(CallFrame* call_frame) {
    check_local_interrupts = false;

    if(run_signals_) {
      shared.signal_handler()->deliver_signals(call_frame);
    }

    switch(thread_state_.raise_reason()) {
    case cException:
      {
        Exception* exc = thread_state_.current_exception();
        if(exc->locations()->nil_p() ||
            exc->locations()->size() == 0) {
          exc->locations(this, Location::from_call_stack(this, call_frame));
        }

        return false;
      }
    case cNone:
      return true;

    default:
      return false;
    }
  }

  void VM::register_raise(Exception* exc) {
    thread_state_.raise_exception(exc);
    check_local_interrupts = true;
    get_attention();
  }

  void VM::check_exception(CallFrame* call_frame) {
    if(thread_state()->raise_reason() == cNone) {
      std::cout << "Exception propogating, but none registered!\n";
      call_frame->print_backtrace(this);
      abort();
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
};
