#ifndef RBX_VM_H
#define RBX_VM_H

#include "missing/time.h"

#include "vm_jit.hpp"
#include "globals.hpp"
#include "memory/object_mark.hpp"
#include "memory/managed.hpp"
#include "vm_thread_state.hpp"
#include "thread_nexus.hpp"
#include "metrics.hpp"

#include "util/thread.hpp"

#include "memory/variable_buffer.hpp"
#include "memory/root_buffer.hpp"
#include "memory/slab.hpp"

#include "shared_state.hpp"

#include "unwind_info.hpp"
#include "fiber_stack.hpp"

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

  namespace memory {
    class GarbageCollector;
    class WriteBarrier;
  }

  class Channel;
  class GlobalCache;
  class Primitives;
  class Memory;
  class TypeInfo;
  class String;
  class Symbol;
  class ConfigParser;
  class TypeError;
  class Assertion;
  struct CallFrame;
  class CallSiteInformation;
  class Object;
  class Configuration;
  class VMManager;
  class LookupTable;
  class SymbolTable;
  class SharedState;
  class Fiber;
  class Park;
  class NativeMethodEnvironment;

  enum MethodMissingReason {
    eNone, ePrivate, eProtected, eSuper, eVCall, eNormal
  };

  enum ConstantMissingReason {
    vFound, vPrivate, vNonExistent
  };

  /**
   * Represents an execution context for running Ruby code.
   *
   * Each Ruby thread is backed by an instance of this class, as well as an
   * instance of the Thread class. Thread manages the (Ruby visible) thread-
   * related state, while this class manages the execution machinery for
   * running Ruby code.
   */

  class VM : public memory::ManagedThread {
    friend class State;
    friend class VMJIT;

  private:
    UnwindInfoSet unwinds_;

    ThreadNexus* thread_nexus_;
    CallFrame* last_frame_;
    CallSiteInformation* saved_call_site_information_;
    FiberStacks fiber_stacks_;
    Park* park_;
    rbxti::Env* tooling_env_;

    uintptr_t root_stack_start_;
    uintptr_t root_stack_size_;

    utilities::thread::SpinLock interrupt_lock_;

    VMJIT vm_jit_;

    MethodMissingReason method_missing_reason_;
    ConstantMissingReason constant_missing_reason_;

    bool zombie_;
    bool tooling_;
    bool allocation_tracking_;
    bool main_thread_;
    bool tracked_;

    ThreadNexus::Phase thread_phase_;

  public:
    /* Data members */
    SharedState& shared;
    memory::TypedRoot<Channel*> waiting_channel_;
    memory::TypedRoot<Exception*> interrupted_exception_;
    /// The Thread object for this VM state
    memory::TypedRoot<Thread*> thread;

    /// The current fiber running on this thread
    memory::TypedRoot<Fiber*> current_fiber;

    /// Root fiber, if any (lazily initialized)
    memory::TypedRoot<Fiber*> root_fiber;

    /// Object that waits for inflation
    memory::TypedRoot<Object*> waiting_object_;

    NativeMethodEnvironment* native_method_environment;

    void (*custom_wakeup_)(void*);
    void* custom_wakeup_data_;

    Memory* memory_;

    VMThreadState thread_state_;

    static unsigned long cStackDepthMax;

  public: /* Inline methods */

    UnwindInfoSet& unwinds() {
      return unwinds_;
    }

    uint32_t thread_id() const {
      return id_;
    }

    ThreadNexus::Phase thread_phase() {
      return thread_phase_;
    }

    void set_tracked(bool tracked = true) {
      tracked_ = tracked;
    }

    bool tracked_p() {
      return tracked_;
    }

    ThreadNexus* thread_nexus() {
      return thread_nexus_;
    }

    void set_thread_phase(ThreadNexus::Phase thread_phase) {
      thread_phase_ = thread_phase;
    }

    utilities::thread::SpinLock& interrupt_lock() {
      return interrupt_lock_;
    }

    void set_zombie(STATE);

    bool zombie_p() {
      return zombie_;
    }

    void set_main_thread() {
      main_thread_ = true;
    }

    bool main_thread_p() {
      return main_thread_;
    }

    VMThreadState* thread_state() {
      return &thread_state_;
    }

    Memory* memory() {
      return shared.memory();
    }

    void set_call_frame(CallFrame* frame) {
      last_frame_ = frame;
    }

    CallFrame* last_frame() const {
      return last_frame_;
    }

    void set_call_site_information(CallSiteInformation* info) {
      saved_call_site_information_ = info;
    }

    CallSiteInformation* saved_call_site_information() {
      return saved_call_site_information_;
    }

    GlobalCache* global_cache() const {
      return shared.global_cache;
    }

    Globals& globals() {
      return shared.globals;
    }

    void* stack_start() const {
      return reinterpret_cast<void*>(vm_jit_.stack_start_);
    }

    int stack_size() const {
      return vm_jit_.stack_size_;
    }

    void reset_stack_limit() {
      // @TODO assumes stack growth direction
      vm_jit_.stack_limit_ = (vm_jit_.stack_start_ - vm_jit_.stack_size_) + (4096 * 10);
    }

    void set_stack_bounds(uintptr_t start, int length) {
      if(start == 0) {
        start  = root_stack_start_;
        length = root_stack_size_;
      }

      vm_jit_.stack_start_ = start;
      vm_jit_.stack_size_ = length;
      reset_stack_limit();
    }

    void set_root_stack(uintptr_t start, int length) {
      root_stack_start_ = start;
      root_stack_size_ = length;

      set_stack_bounds(root_stack_start_, root_stack_size_);
    }

    bool detect_stack_condition(void* end) const {
      // @TODO assumes stack growth direction
      return reinterpret_cast<uintptr_t>(end) < vm_jit_.stack_limit_;
    }

    MethodMissingReason method_missing_reason() const {
      return method_missing_reason_;
    }

    void set_method_missing_reason(MethodMissingReason reason) {
      method_missing_reason_ = reason;
    }

    ConstantMissingReason constant_missing_reason() const {
      return constant_missing_reason_;
    }

    void set_constant_missing_reason(ConstantMissingReason reason) {
      constant_missing_reason_ = reason;
    }

    void after_fork_child(STATE);

    bool thread_step() const {
      return vm_jit_.thread_step_;
    }

    void clear_thread_step() {
      clear_check_local_interrupts();
      vm_jit_.thread_step_ = false;
    }

    void set_thread_step() {
      set_check_local_interrupts();
      vm_jit_.thread_step_ = true;
    }

    bool check_local_interrupts() const {
      return vm_jit_.check_local_interrupts_;
    }

    void clear_check_local_interrupts() {
      vm_jit_.check_local_interrupts_ = false;
    }

    void set_check_local_interrupts() {
      vm_jit_.check_local_interrupts_ = true;
    }

    bool interrupt_by_kill() const {
      return vm_jit_.interrupt_by_kill_;
    }

    void clear_interrupt_by_kill() {
      vm_jit_.interrupt_by_kill_ = false;
    }

    void set_interrupt_by_kill() {
      vm_jit_.interrupt_by_kill_ = true;
    }

    Exception* interrupted_exception() const {
      return interrupted_exception_.get();
    }

    void clear_interrupted_exception() {
      interrupted_exception_.set(cNil);
    }

    rbxti::Env* tooling_env() const {
      return tooling_env_;
    }

    bool tooling() const {
      return tooling_;
    }

    void enable_tooling() {
      tooling_ = true;
    }

    void disable_tooling() {
      tooling_ = false;
    }

    bool allocation_tracking() const {
      return allocation_tracking_;
    }

    void enable_allocation_tracking() {
      allocation_tracking_ = true;
    }

    void disable_allocation_tracking() {
      allocation_tracking_ = false;
    }

    FiberStack* allocate_fiber_stack() {
      return fiber_stacks_.allocate();
    }

    void* fiber_trampoline() {
      return fiber_stacks_.trampoline();
    }

    FiberData* new_fiber_data(bool root=false) {
      return fiber_stacks_.new_data(root);
    }

    void remove_fiber_data(FiberData* data) {
      fiber_stacks_.remove_data(data);
    }

    memory::VariableRootBuffers& current_root_buffers();

  public:
    static void init_stack_size();

    static VM* current();

    static void discard(STATE, VM*);

  public:

    /* Prototypes */
    VM(uint32_t id, SharedState& shared, const char* name = NULL);
    ~VM();

    void bootstrap_class(STATE);
    void bootstrap_ontology(STATE);
    void bootstrap_symbol(STATE);
    void initialize_config();

    void collect_maybe(STATE);

    void checkpoint(STATE, CallFrame* call_frame) {
      metrics().machine.checkpoints++;

      if(thread_nexus_->stop_lock(this)) {
        metrics().machine.stops++;

        set_call_frame(call_frame);
        collect_maybe(state);

        thread_nexus_->unlock();
      }
    }

    void become_managed();
    void become_unmanaged() {
      thread_phase_ = ThreadNexus::cUnmanaged;
    }

    void set_current_thread();

    void setup_errno(STATE, int num, const char* name, Class* sce, Module* ern);
    void bootstrap_exceptions(STATE);
    void initialize_fundamental_constants(STATE);
    void initialize_builtin_classes(STATE);
    void initialize_platform_data(STATE);
    Object* ruby_lib_version();

    void set_current_fiber(Fiber* fib);

    TypeInfo* find_type(int type);

    static void init_ffi(STATE);

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

    void wait_on_channel(Channel* channel);
    void wait_on_inflated_lock(Object* wait);
    void wait_on_custom_function(void (*func)(void*), void* data);
    void clear_waiter();
    bool wakeup(STATE, CallFrame* call_frame);

    void reset_parked();

    void set_sleeping();
    void clear_sleeping();

    void interrupt_with_signal();
    bool should_interrupt_with_signal() const {
      return vm_jit_.interrupt_with_signal_;
    }

    void register_raise(STATE, Exception* exc);
    void register_kill(STATE);

    void gc_scan(memory::GarbageCollector* gc);
    void gc_fiber_clear_mark();
    void gc_fiber_scan(memory::GarbageCollector* gc, bool only_marked = true);
    void gc_verify(memory::GarbageCollector* gc);
  };
}

#endif
