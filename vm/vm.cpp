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
      local_slab_.refill(0, 0);
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

    local_slab_.refill(0, 0);

    shared.set_initialized();

    shared.gc_dependent(this);

    State state(this);

    TypeInfo::auto_learn_fields(&state);

    bootstrap_ontology(&state);

    VMMethod::init(&state);

    // Setup the main Thread, which is wrapper of the main native thread
    // when the VM boots.
    thread.set(Thread::create(&state, this, G(thread), 0, true), &globals().roots);
    thread->sleep(&state, Qfalse);

    VM::set_current(this);
  }

  void VM::initialize_config() {
    State state(this);

#ifdef ENABLE_LLVM
    if(!shared.config.jit_disabled) {
      Array* ary = Array::create(&state, 3);
      ary->append(&state, symbol("usage"));
      if(shared.config.jit_inline_generic) {
        ary->append(&state, symbol("inline_generic"));
      }

      if(shared.config.jit_inline_blocks) {
        ary->append(&state, symbol("inline_blocks"));
      }
      G(rubinius)->set_const(&state, "JIT", ary);
    } else {
      G(rubinius)->set_const(&state, "JIT", Qfalse);
    }
#else
    G(rubinius)->set_const(&state, "JIT", Qnil);
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
    State state(this);

    if(unlikely(size > om->large_object_threshold)) {
      return om->new_object_typed_enduring(&state, cls, size, type);
    }

    Object* obj = local_slab().allocate(size).as<Object>();

    if(unlikely(!obj)) {
      if(shared.om->refill_slab(&state, local_slab())) {
        obj = local_slab().allocate(size).as<Object>();
      }

      // If refill_slab fails, obj will still be NULL.

      if(!obj) {
        return om->new_object_typed(&state, cls, size, type);
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
      State state(this);

      if(shared.om->refill_slab(&state, local_slab())) {
        tup = local_slab().allocate(bytes).as<Tuple>();
      }

      if(!tup) return 0;
    }

    tup->init_header(G(tuple), YoungObjectZone, Tuple::type);
    tup->full_size_ = bytes;

    return tup;
  }

  Object* VM::new_object_typed_mature(Class* cls, size_t bytes, object_type type) {
    State state(this);

    return om->new_object_typed_mature(&state, cls, bytes, type);
  }

  Symbol* VM::symbol(const char* str) {
    return shared.symbols.lookup(str, strlen(str));
  }

  Symbol* VM::symbol(std::string str) {
    State state(this);
    return shared.symbols.lookup(&state, str);
  }

  Symbol* VM::symbol(String* str) {
    State state(this);
    return shared.symbols.lookup(&state, str);
  }

  void type_assert(STATE, Object* obj, object_type type, const char* reason) {
    if((obj->reference_p() && obj->type_id() != type)
        || (type == FixnumType && !obj->fixnum_p())) {
      Exception::type_error(state, type, obj, reason);
    }
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
    shared.gc_soon();
  }

  void VM::collect(GCToken gct, CallFrame* call_frame) {
    State state(this);
    this->set_call_frame(call_frame);
    om->collect(&state, gct, call_frame);
  }

  void VM::collect_maybe(GCToken gct, CallFrame* call_frame) {
    State state(this);
    this->set_call_frame(call_frame);
    om->collect_maybe(&state, gct, call_frame);
  }

  void VM::set_const(const char* name, Object* val) {
    State state(this);
    globals().object->set_const(&state, (char*)name, val);
  }

  void VM::set_const(Module* mod, const char* name, Object* val) {
    State state(this);
    mod->set_const(&state, (char*)name, val);
  }

  Object* VM::path2class(const char* path) {
    State state(this);
    Module* mod = shared.globals.object.get();

    char* copy = strdup(path);
    char* cur = copy;

    for(;;) {
      char* pos = strstr(cur, "::");
      if(pos) *pos = 0;

      Object* obj = mod->get_const(&state, symbol(cur));

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

  bool VM::wakeup(STATE, GCToken gct) {
    SYNC(state);

    check_local_interrupts = true;

    // Wakeup any locks hanging around with contention
    om->release_contention(state, gct);

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
        chan->send(state, gct, Qnil);
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
    SYNC_TL;
    interrupt_with_signal_ = false;
    waiting_channel_.set((Channel*)Qnil);
    waiting_header_ = 0;
    custom_wakeup_ = 0;
    custom_wakeup_data_ = 0;
  }

  void VM::wait_on_channel(Channel* chan) {
    SYNC_TL;
    thread->sleep(this, Qtrue);
    waiting_channel_.set(chan);
  }

  void VM::wait_on_inflated_lock(InflatedHeader* ih) {
    SYNC_TL;
    waiting_header_ = ih;
  }

  void VM::wait_on_custom_function(void (*func)(void*), void* data) {
    SYNC_TL;
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

  void VM::register_raise(STATE, Exception* exc) {
    SYNC(state);
    interrupted_exception_.set(exc);
    check_local_interrupts = true;
    get_attention();
  }

  void VM::set_current_fiber(Fiber* fib) {
    set_stack_start(fib->stack());
    set_stack_size(fib->stack_size());
    current_fiber.set(fib);
  }

  GCIndependent::GCIndependent(NativeMethodEnvironment* env)
    : state_(env->state())
  {
    state_->set_call_frame(env->current_call_frame());
    state_->gc_independent();
  };
  
};
