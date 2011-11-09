#ifndef RBX_VM_H
#define RBX_VM_H

#include "missing/time.h"

#include "globals.hpp"
#include "gc/object_mark.hpp"
#include "gc/managed.hpp"
#include "thread_state.hpp"

#include "util/refcount.hpp"
#include "util/thread.hpp"

#include "call_frame_list.hpp"

#include "gc/variable_buffer.hpp"
#include "gc/root_buffer.hpp"
#include "gc/slab.hpp"

#include "shared_state.hpp"

#include <vector>
#include <setjmp.h>

namespace llvm {
  class Module;
}

namespace rbxti {
  class Env;
}

namespace rubinius {

  class Exception;
  class LLVMState;

  namespace event {
    class Loop;
  }

  namespace gc {
    class WriteBarrier;
  }

  class Channel;
  class GlobalCache;
  class Primitives;
  class ObjectMemory;
  class TypeInfo;
  class String;
  class Symbol;
  class ConfigParser;
  class TypeError;
  class Assertion;
  struct CallFrame;
  class Object;
  class Configuration;
  class VMManager;
  class LookupTable;
  class SymbolTable;
  class SharedState;
  class Fiber;

  enum MethodMissingReason {
    eNone, ePrivate, eProtected, eSuper, eVCall, eNormal
  };


  /**
   * Represents an execution context for running Ruby code.
   *
   * Each Ruby thread is backed by an instance of this class, as well as an
   * instance of the Thread class. Thread manages the (Ruby visible) thread-
   * related state, while this class manages the execution machinery for
   * running Ruby code.
   */

  class VM : public ManagedThread {
    friend class State;

  private:
    CallFrame* saved_call_frame_;
    uintptr_t stack_start_;
    uintptr_t stack_limit_;
    int stack_size_;
    bool run_signals_;

    MethodMissingReason method_missing_reason_;
    void* young_start_;
    void* young_end_;
    bool thread_step_;

    rbxti::Env* tooling_env_;
    bool tooling_;

  public:
    /* Data members */
    SharedState& shared;
    TypedRoot<Channel*> waiting_channel_;
    TypedRoot<Exception*> interrupted_exception_;

    bool interrupt_with_signal_;
    InflatedHeader* waiting_header_;

    void (*custom_wakeup_)(void*);
    void* custom_wakeup_data_;

    ObjectMemory* om;

    bool check_local_interrupts;

    ThreadState thread_state_;

    /// The Thread object for this VM state
    TypedRoot<Thread*> thread;

    /// The current fiber running on this thread
    TypedRoot<Fiber*> current_fiber;

    /// Root fiber, if any (lazily initialized)
    TypedRoot<Fiber*> root_fiber;

    static unsigned long cStackDepthMax;

  public: /* Inline methods */

    uint32_t thread_id() {
      return id_;
    }

    bool run_signals_p() {
      return run_signals_;
    }

    void set_run_signals(bool val) {
      run_signals_ = val;
    }

    ThreadState* thread_state() {
      return &thread_state_;
    }

    CallFrame** call_frame_location() {
      return &saved_call_frame_;
    }

    void set_call_frame(CallFrame* frame) {
      saved_call_frame_ = frame;
    }

    CallFrame* saved_call_frame() {
      return saved_call_frame_;
    }

    GlobalCache* global_cache() {
      return shared.global_cache;
    }

    Globals& globals() {
      return shared.globals;
    }

    void* stack_start() {
      return reinterpret_cast<void*>(stack_start_);
    }

    int stack_size() {
      return stack_size_;
    }

    void reset_stack_limit() {
      // @TODO assumes stack growth direction
      stack_limit_ = (stack_start_ - stack_size_) + (4096 * 3);
    }

    void set_stack_bounds(uintptr_t start, int length) {
      stack_start_ = start;
      stack_size_ = length;
      reset_stack_limit();
    }

    void set_stack_start(void* s) {
      set_stack_bounds(reinterpret_cast<uintptr_t>(s), stack_size_);
    }

    void set_stack_size(int s) {
      set_stack_bounds(stack_start_, s);
    }

    void get_attention() {
      stack_limit_ = stack_start_;
    }

    bool detect_stack_condition(void* end) {
      // @TODO assumes stack growth direction
      return reinterpret_cast<uintptr_t>(end) < stack_limit_;
    }

    MethodMissingReason method_missing_reason() {
      return method_missing_reason_;
    }

    void set_method_missing_reason(MethodMissingReason reason) {
      method_missing_reason_ = reason;
    }

    bool young_object_p(Object* obj) {
      return obj >= young_start_ && obj <= young_end_;
    }

    bool thread_step() {
      return thread_step_;
    }

    void clear_thread_step() {
      thread_step_ = false;
    }

    void set_thread_step() {
      thread_step_ = true;
    }

    Exception* interrupted_exception() {
      return interrupted_exception_.get();
    }

    void clear_interrupted_exception() {
      interrupted_exception_.set(Qnil);
    }

    rbxti::Env* tooling_env() {
      return tooling_env_;
    }

    bool tooling() {
      return tooling_;
    }

    void enable_tooling() {
      tooling_ = true;
    }

    void disable_tooling() {
      tooling_ = false;
    }

  public:
    static void init_stack_size();

    static VM* current();
    static void set_current(VM* vm);

    static void discard(STATE, VM*);

  public:

    /* Prototypes */
    VM(uint32_t id, SharedState& shared);

    void initialize_as_root();

    void bootstrap_class(STATE);
    void bootstrap_ontology(STATE);
    void bootstrap_symbol(STATE);
    void initialize_config();

    void setup_errno(STATE, int num, const char* name, Class* sce, Module* ern);
    void bootstrap_exceptions(STATE);
    void initialize_fundamental_constants(STATE);
    void initialize_builtin_classes(STATE);
    void initialize_platform_data(STATE);

    void set_current_fiber(Fiber* fib);

    void raise_stack_error(CallFrame* call_frame);

    Object* new_object_typed(Class* cls, size_t bytes, object_type type);
    Object* new_object_typed_mature(Class* cls, size_t bytes, object_type type);

    template <class T>
      T* new_object(Class *cls) {
        return reinterpret_cast<T*>(new_object_typed(cls, sizeof(T), T::type));
      }

    template <class T>
      T* new_struct(Class* cls, size_t bytes = 0) {
        T* obj = reinterpret_cast<T*>(new_object_typed(cls, sizeof(T) + bytes, T::type));
        return obj;
      }

    template <class T>
      T* new_object_mature(Class *cls) {
        return reinterpret_cast<T*>(new_object_typed_mature(cls, sizeof(T), T::type));
      }

    template <class T>
      T* new_object_bytes(Class* cls, size_t& bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = reinterpret_cast<T*>(new_object_typed(cls, bytes, T::type));

        return obj;
      }

    template <class T>
      T* new_object_variable(Class* cls, size_t fields, size_t& bytes) {
        bytes = sizeof(T) + (fields * sizeof(Object*));
        return reinterpret_cast<T*>(new_object_typed(cls, bytes, T::type));
      }

    /// Create a Tuple in the young GC space, return NULL if not possible.
    Tuple* new_young_tuple_dirty(size_t fields);

    Symbol* symbol(const char* str);
    Symbol* symbol(std::string str);
    Symbol* symbol(String* str);

    TypeInfo* find_type(int type);

    void init_ffi(STATE);
    void init_native_libraries();

    Thread* current_thread();
    void collect(GCToken gct, CallFrame* call_frame);

    /// Check the GC flags in ObjectMemory and collect if we need to.
    void collect_maybe(GCToken gct, CallFrame* call_frame);

    void raise_from_errno(const char* reason);
    void raise_exception(Exception* exc);
    Exception* new_exception(Class* cls, const char* msg);
    Object* current_block();

    void set_const(const char* name, Object* val);
    void set_const(Module* mod, const char* name, Object* val);

    Object* path2class(const char* name);

#ifdef ENABLE_LLVM
    llvm::Module* llvm_module();
    void llvm_cleanup();
#endif

    void print_backtrace();


    /// Run the garbage collectors as soon as you can
    void run_gc_soon();

    void wait_on_channel(Channel* channel);
    void wait_on_inflated_lock(InflatedHeader* ih);
    void wait_on_custom_function(void (*func)(void*), void* data);
    void clear_waiter();
    bool wakeup(STATE, GCToken gct);
    bool waiting_p();

    void set_sleeping();
    void clear_sleeping();

    void interrupt_with_signal();
    bool should_interrupt_with_signal() {
      return interrupt_with_signal_;
    }

    void register_raise(STATE, Exception* exc);

    // For thread-local roots
    static std::list<Roots*>* roots;
  };

}

#include "state.hpp"

namespace rubinius {


  /**
   * Instantiation of an instance of this class causes Ruby execution on all
   * threads to be suspended. Upon destruction of the instance, Ruby execution
   * is resumed.
   */

  class StopTheWorld {
    State* state_;

  public:
    StopTheWorld(STATE) :
      state_(state)
    {
      state->stop_the_world();
    }

    ~StopTheWorld() {
      state_->restart_world();
    }
  };

  class NativeMethodEnvironment;

  class GCIndependent {
    State* state_;

  public:
    GCIndependent(STATE, CallFrame* call_frame)
      : state_(state)
    {
      state_->set_call_frame(call_frame);
      state_->gc_independent();
    }

    GCIndependent(STATE)
      : state_(state)
    {
      state_->gc_independent();
    }
    
    GCIndependent(NativeMethodEnvironment* env);

    ~GCIndependent() {
      state_->gc_dependent();
    }
  };

  template <class T>
  class GCIndependentLockGuard : public thread::LockGuardTemplate<T> {
    State* state_;
  public:
    GCIndependentLockGuard(STATE, GCToken gct, T& in_lock)
      : thread::LockGuardTemplate<T>(in_lock, false)
      , state_(state)
    {
      state_->shared().gc_independent(state_);
      this->lock();
      state->shared().gc_dependent(state_);
    }

    ~GCIndependentLockGuard() {
      this->unlock();
    }
  };

   typedef GCIndependentLockGuard<thread::Mutex> GCLockGuard;
};

#endif
