#ifndef RBX_VM_H
#define RBX_VM_H

#include "globals.hpp"
#include "gc/object_mark.hpp"
#include "gc/managed.hpp"
#include "thread_state.hpp"

#include "util/refcount.hpp"

#include "global_lock.hpp"
#include "maps.hpp"

#include "call_frame_list.hpp"

#include "async_message.hpp"
#include "gc/variable_buffer.hpp"

#include "shared_state.hpp"

#include <vector>
#include <pthread.h>
#include <setjmp.h>

namespace llvm {
  class Module;
}

namespace rubinius {

  class Exception;
  class LLVMState;

  namespace event {
    class Loop;
  }

  namespace profiler {
    class Profiler;
  }

  namespace gc {
    class WriteBarrier;
  }

  class GlobalCache;
  class TaskProbe;
  class Primitives;
  class ObjectMemory;
  class TypeInfo;
  class MethodContext;
  class String;
  class Symbol;
  class ConfigParser;
  class TypeError;
  class Assertion;
  struct CallFrame;
  class Object;
  class Configuration;
  struct Interrupts;
  class VMManager;
  class Waiter;
  class LookupTable;
  class SymbolTable;
  class SharedState;
  class Fiber;

  enum MethodMissingReason {
    eNone, ePrivate, eProtected, eSuper, eVCall, eNormal
  };

  class VM : public ManagedThread {
  private:
    CallFrame* saved_call_frame_;
    void* stack_start_;
    uintptr_t stack_limit_;
    int stack_size_;
    profiler::Profiler* profiler_;
    bool run_signals_;

    MethodMissingReason method_missing_reason_;

  public:
    /* Data members */
    SharedState& shared;
    thread::Mutex local_lock_;
    Waiter* waiter_;
    bool interrupt_with_signal_;
    pthread_t os_thread_;

    ObjectMemory* om;
    TypedRoot<TaskProbe*> probe;
    Interrupts& interrupts;

    bool check_local_interrupts;

    ThreadState thread_state_;

    // The Thread object for this VM state
    TypedRoot<Thread*> thread;

    // The current fiber running on this thread
    TypedRoot<Fiber*> current_fiber;

    static unsigned long cStackDepthMax;

  public: /* Inline methods */

    bool run_signals_p() {
      return run_signals_;
    }

    void set_run_signals(bool val) {
      run_signals_ = val;
    }

    ThreadState* thread_state() {
      return &thread_state_;
    }

    GlobalLock& global_lock() {
      return shared.global_lock();
    }

    thread::Mutex& local_lock() {
      return local_lock_;
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

    // NOTE this will need to be VM local, ie Thread local, once the GIL
    // is removed.
    VariableRootBuffers* variable_buffers() {
      return shared.variable_buffers();
    }

    GlobalCache* global_cache() {
      return shared.global_cache;
    }

    Globals& globals() {
      return shared.globals;
    }

    void* stack_start() {
      return stack_start_;
    }

    void set_stack_start(void* s) {
      stack_start_ = s;
      // @TODO assumes stack growth direction
      stack_limit_ = (reinterpret_cast<uintptr_t>(s) - stack_size_) + 4096;
    }

    int stack_size() {
      return stack_size_;
    }

    void set_stack_size(int s) {
      stack_size_ = s;
      // @TODO assumes stack growth direction
      stack_limit_ = (reinterpret_cast<uintptr_t>(stack_start_) - s) + 4096;
    }

    void set_stack_bounds(void* start, int length) {
      stack_start_ = start;
      stack_size_ = length;
      // @TODO assumes stack growth direction
      stack_limit_ = (reinterpret_cast<uintptr_t>(start) - length) + (4096 * 3);
    }

    bool check_stack(CallFrame* call_frame, void* end) {
      // @TODO assumes stack growth direction
      if(unlikely(reinterpret_cast<uintptr_t>(end) < stack_limit_)) {
        raise_stack_error(call_frame);
        return false;
      }

      return true;
    }

    MethodMissingReason method_missing_reason() {
      return method_missing_reason_;
    }

    void set_method_missing_reason(MethodMissingReason reason) {
      method_missing_reason_ = reason;
    }

  public:
    static void init_stack_size();

    // Better than current_state, uses a pthread local.
    static VM* current();
    static void set_current(VM* vm);

    // Returns the current VM state object.
    static VM* current_state();

    // Registers a VM* object as the current state.
    static void register_state(VM*);

    static void discard(VM*);

  public:

    /* Prototypes */
    VM(SharedState& shared);

    void check_exception(CallFrame* call_frame);

    void initialize();

    // Initialize the basic objects and the execution machinery
    void boot();

    void bootstrap_class();
    void bootstrap_ontology();
    void bootstrap_symbol();
    void initialize_config();

    void setup_errno(int num, const char* name, Class* sce, Module* ern);
    void bootstrap_exceptions();
    void initialize_fundamental_constants();
    void initialize_builtin_classes();
    void initialize_platform_data();
    void boot_threads();

    void set_current_fiber(Fiber* fib);

    void raise_stack_error(CallFrame* call_frame);

    Object* new_object_typed(Class* cls, size_t bytes, object_type type);
    Object* new_object_typed_mature(Class* cls, size_t bytes, object_type type);
    Object* new_object_from_type(Class* cls, TypeInfo* ti);

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

    // Create an uninitialized Class object
    Class* new_basic_class(Class* sup);

    // Create a Class of name +name+ as an Object subclass
    Class* new_class(const char* name);

    // Create a Class of name +name+ as a subclass of +super_class+
    Class* new_class(const char* name, Class* super_class);

    // Create a Class of name +name+ as a subclass of +sup+
    // under Module +under+
    Class* new_class(const char* name, Class* sup, Module* under);

    // Create a Class of name +name+ under +under+
    Class* new_class_under(const char* name, Module* under);

    Module* new_module(const char* name, Module* under = NULL);

    Symbol* symbol(const char* str);
    Symbol* symbol(String* str);

    TypeInfo* find_type(int type);

    void init_ffi();
    void init_native_libraries();

    Thread* current_thread();
    void collect(CallFrame* call_frame);

    // Check the flags in ObjectMemory and collect if we need to.
    void collect_maybe(CallFrame* call_frame);

    void raise_from_errno(const char* reason);
    void raise_exception(Exception* exc);
    Exception* new_exception(Class* cls, const char* msg);
    Object* current_block();

    void set_const(const char* name, Object* val);
    void set_const(Module* mod, const char* name, Object* val);

#ifdef ENABLE_LLVM
    llvm::Module* llvm_module();
    void llvm_cleanup();
#endif

    void print_backtrace();


    // Run the garbage collectors as soon as you can
    void run_gc_soon();

    void install_waiter(Waiter& waiter);
    void clear_waiter();
    bool wakeup();

    void interrupt_with_signal();
    bool should_interrupt_with_signal() {
      return interrupt_with_signal_;
    }

    bool waiting_p() {
      return interrupt_with_signal_ || waiter_ != NULL;
    }

    void register_raise(Exception* exc);

    bool process_async(CallFrame* call_frame);

    bool check_async(CallFrame* call_frame) {
      if(check_local_interrupts) {
        return process_async(call_frame);
      }
      return true;
    }

    profiler::Profiler* profiler();

    void remove_profiler();

    // For thread-local roots
    static std::list<Roots*>* roots;
  };

  class StopTheWorld {
    VM* vm_;

  public:
    StopTheWorld(STATE) :
      vm_(state)
    {
      vm_->shared.stop_the_world();
    }

    ~StopTheWorld() {
      vm_->shared.restart_world();
    }
  };
};

#endif
