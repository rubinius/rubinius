#ifndef RBX_VM_H
#define RBX_VM_H

#include "globals.hpp"
#include "symboltable.hpp"
#include "gc/object_mark.hpp"
#include "thread_state.hpp"

#include "util/refcount.hpp"

#include "global_lock.hpp"
#include "maps.hpp"

#include "call_frame_list.hpp"

#include "async_message.hpp"
#include "gc/variable_buffer.hpp"

#include <vector>
#include <pthread.h>
#include <setjmp.h>

namespace llvm {
  class Module;
}

namespace rubinius {

  class Exception;

  namespace event {
    class Loop;
  }

  namespace profiler {
    class Profiler;
    class ProfilerCollection;
  }

  namespace capi {
    class Handles;
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
  class CallFrame;
  class Object;

  struct Configuration {
    bool compile_up_front;
    bool jit_enabled;
    bool dynamic_interpreter_enabled;
  };

  struct Interrupts {
    bool check;
    bool switch_task;
    bool perform_gc;
    bool check_events;
    bool reschedule;
    bool use_preempt;
    bool enable_preempt;
    bool timer;

    Interrupts() :
      check(false),
      switch_task(false),
      perform_gc(false),
      check_events(false),
      reschedule(false),
      use_preempt(false),
      enable_preempt(false),
      timer(false)
    { }
  };

  struct Stats {
    // How much time is spent running the JIT
    uint64_t jit_timing;

    // How many methods have been compiled by the JIT
    uint64_t jitted_methods;

    // How much time is spent in the GC
    uint64_t time_in_gc;

    Stats()
      : jit_timing(0)
      , jitted_methods(0)
      , time_in_gc(0)
    {}
  };

  class VMManager;
  class Waiter;
  class SignalThread;
  class LookupTable;

  class SharedState : public RefCount {
  private:
    VMManager& manager_;
    bool initialized_;
    int id_;
    GlobalLock lock_;
    VMMap vms_;
    SignalThread* signal_thread_;
    CallFrameLocationList cf_locations_;
    VariableRootBuffers root_buffers_;
    capi::Handles* global_handles_;
    bool profiling_;
    profiler::ProfilerCollection* profiler_collection_;

  public:
    Globals globals;
    ObjectMemory* om;
    GlobalCache* global_cache;
    Configuration config;
    Interrupts interrupts;
    SymbolTable symbols;
    ConfigParser *user_config;

  public:
    SharedState(VMManager& manager, int id);
    ~SharedState();

    int id() {
      return id_;
    }

    VMManager& manager() {
      return manager_;
    }

    void set_initialized() {
      initialized_ = true;
    }

    GlobalLock& global_lock() {
      return lock_;
    }

    SignalThread* signal_thread() {
      return signal_thread_;
    }

    void set_signal_thread(SignalThread* thr) {
      signal_thread_ = thr;
    }

    static SharedState* standalone(VM*);
    VM* new_vm();
    void remove_vm(VM*);

    CallFrameLocationList& call_frame_locations() {
      return cf_locations_;
    }

    VariableRootBuffers* variable_buffers() {
      return &root_buffers_;
    }

    capi::Handles* global_handles() {
      return global_handles_;
    }

    bool profiling() {
      return profiling_;
    }

    void enable_profiling(VM* vm);

    LookupTable* disable_profiling(VM* vm);

    void add_profiler(VM* vm, profiler::Profiler* profiler);

    void remove_profiler(VM* vm, profiler::Profiler* profiler);
  };

  class VM {

  private:
    int id_;
    CallFrame* saved_call_frame_;
    ASyncMessageMailbox mailbox_;
    void* stack_start_;
    bool alive_;
    profiler::Profiler* profiler_;

  public:
    /* Data members */
    SharedState& shared;
    thread::Mutex local_lock_;
    Waiter* waiter_;

    ConfigParser *user_config;
    Globals& globals;
    ObjectMemory* om;
    event::Loop* events;
    event::Loop* signal_events;
    GlobalCache* global_cache;
    TypedRoot<TaskProbe*> probe;
    Configuration& config;
    Interrupts& interrupts;
    SymbolTable& symbols;

    bool check_local_interrupts;

    ThreadState thread_state_;

    // The Thread object for this VM state
    TypedRoot<Thread*> thread;

    Stats stats;

    // Temporary holder for rb_gc_mark() in subtend
    ObjectMark current_mark;

    bool reuse_llvm;

    // The thread used to trigger preemptive thread switching
    pthread_t preemption_thread;

    // The safe position on the stack used to handle rare
    // events.
    sigjmp_buf safe_position;

    // Indicates if safe_position should be used, or if the error
    // should be thrown as a C++ exception.
    bool use_safe_position;

    // Data used with safe_position.
    union {
      Exception* exc;
      TypeError* type_error;
      Assertion* assertion;
    } safe_position_data;

    static const int cReasonException = 1;
    static const int cReasonTypeError = 2;
    static const int cReasonAssertion = 3;

    static const size_t default_bytes = 1048576 * 3;

    static int cStackDepthMax;

  public: /* Inline methods */

    int id() {
      return id_;
    }

    bool alive_p() {
      return alive_;
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

    void* stack_start() {
      return stack_start_;
    }

    void set_stack_start(void* s) {
      stack_start_ = s;
    }

    bool check_stack(CallFrame* call_frame, void* end) {
      // @TODO assumes stack growth direction
      if(reinterpret_cast<intptr_t>(stack_start_) -
          reinterpret_cast<intptr_t>(end) > cStackDepthMax) {
        raise_stack_error(call_frame);
        return false;
      }

      return true;
    }

    void check_exception(CallFrame* call_frame);

    /* Prototypes */
    VM(SharedState& shared, int id);

    void initialize(size_t bytes = default_bytes);

    // Initialize the basic objects and the execution machinery
    void boot();

    // Returns the current VM state object.
    static VM* current_state();

    // Registers a VM* object as the current state.
    static void register_state(VM*);

    void discard();

    void bootstrap_class();
    void bootstrap_ontology();
    void bootstrap_symbol();
    void bootstrap_exceptions();
    void initialize_fundamental_constants();
    void initialize_builtin_classes();
    void initialize_platform_data();
    void boot_threads();

    void raise_exception_safely(Exception* exc);
    void raise_typeerror_safely(TypeError* exc);
    void raise_assertion_safely(Assertion* exc);

    void raise_stack_error(CallFrame* call_frame);
    void init_stack_size();

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
    Symbol* symbol(std::string str);

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

    void setup_preemption();

    // Run in a seperate thread to provide preemptive thread
    // scheduling.
    void scheduler_loop();

    // Run the garbage collectors as soon as you can
    void run_gc_soon();

    void install_waiter(Waiter& waiter);
    void clear_waiter();
    bool wakeup();

    void register_raise(Exception* exc);

    // Called when a thread should be delivered to this thread
    void send_async_signal(int sig);

    bool process_async(CallFrame* call_frame);

    bool check_async(CallFrame* call_frame) {
      if(check_local_interrupts) {
        return process_async(call_frame);
      }
      return true;
    }

    profiler::Profiler* profiler();

    void remove_profiler();
  };
};

#endif
