#ifndef RBX_VM_H
#define RBX_VM_H

#include "missing/time.h"

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

#include "sodium/randombytes.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <regex>
#include <string>
#include <thread>
#include <vector>
#include <setjmp.h>
#include <stdint.h>

namespace rubinius {
  class Fiber;
  class Exception;

  namespace event {
    class Loop;
  }

  namespace memory {
    class GarbageCollector;
    class WriteBarrier;
  }

  class Assertion;
  class CallSiteInformation;
  class Channel;
  class CompiledCode;
  class ConfigParser;
  class Configuration;
  class GlobalCache;
  class LookupTable;
  class Memory;
  class NativeMethodEnvironment;
  class Object;
  class Park;
  class Primitives;
  class SharedState;
  class String;
  class Symbol;
  class SymbolTable;
  class Tuple;
  class TypeError;
  class TypeInfo;
  class VariableScope;

  struct CallFrame;

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

  private:
    static const int cWaitLimit = 100;

    UnwindInfoSet unwinds_;

    CallFrame* call_frame_;
    ThreadNexus* thread_nexus_;
    Park* park_;

    void* stack_start_;
    size_t stack_size_;
    size_t stack_cushion_;

    bool interrupt_with_signal_;
    bool interrupt_by_kill_;
    bool check_local_interrupts_;
    bool thread_step_;

    std::mutex fiber_wait_mutex_;
    std::condition_variable fiber_wait_condition_;

    enum FiberTransition {
      eSuspending,
      eSuspended,
      eResuming,
      eRunning,
      eCanceled,
      eFinished
    };

    std::atomic<FiberTransition> fiber_transition_flag_;

    utilities::thread::SpinLock interrupt_lock_;

    MethodMissingReason method_missing_reason_;
    ConstantMissingReason constant_missing_reason_;

    bool zombie_;
    bool allocation_tracking_;
    bool main_thread_;

    std::atomic<ThreadNexus::Phase> thread_phase_;

    uint32_t profile_interval_;
    uint32_t profile_counter_;
    CompiledCode** profile_;
    uint64_t profile_sample_count_;
    uint64_t profile_report_interval_;
    native_int max_profile_entries_;
    native_int min_profile_sample_count_;

  public:
    /* Data members */
    SharedState& shared;
    memory::TypedRoot<Channel*> waiting_channel_;
    memory::TypedRoot<Exception*> interrupted_exception_;
    /// The Thread object for this VM state
    memory::TypedRoot<Thread*> thread_;
    memory::TypedRoot<Fiber*> fiber_;

    /// Object that waits for inflation
    memory::TypedRoot<Object*> waiting_object_;

    uint64_t start_time_;

    NativeMethodEnvironment* native_method_environment;

    void (*custom_wakeup_)(void*);
    void* custom_wakeup_data_;

    VMThreadState thread_state_;

  public: /* Inline methods */

    UnwindInfoSet& unwinds() {
      return unwinds_;
    }

    uint32_t thread_id() const {
      return id_;
    }

    ThreadNexus* thread_nexus() {
      return thread_nexus_;
    }

    ThreadNexus::Phase thread_phase() {
      return thread_phase_.load(std::memory_order_acquire);
    }

    void set_thread_phase(ThreadNexus::Phase thread_phase) {
      thread_phase_.store(thread_phase, std::memory_order_release);
    }

    utilities::thread::SpinLock& interrupt_lock() {
      return interrupt_lock_;
    }

    std::mutex& fiber_wait_mutex() {
      return fiber_wait_mutex_;
    }

    std::condition_variable& fiber_wait_condition() {
      return fiber_wait_condition_;
    }

    FiberTransition fiber_transition_flag() {
      return fiber_transition_flag_;
    }

    bool suspending_p() const {
      return fiber_transition_flag_ == eSuspending;
    }

    bool suspended_p() const {
      return fiber_transition_flag_ == eSuspended;
    }

    bool resuming_p() const {
      return fiber_transition_flag_ == eResuming;
    }

    bool running_p() const {
      return fiber_transition_flag_ == eRunning;
    }

    bool canceled_p() const {
      return fiber_transition_flag_ == eCanceled;
    }

    bool finished_p() const {
      return fiber_transition_flag_ == eFinished;
    }

    void set_suspending() {
      fiber_transition_flag_ = eSuspending;
    }

    void set_suspended() {
      fiber_transition_flag_ = eSuspended;
    }

    void set_resuming() {
      fiber_transition_flag_ = eResuming;
    }

    void set_running() {
      fiber_transition_flag_ = eRunning;
    }

    void set_canceled() {
      fiber_transition_flag_ = eCanceled;
    }

    void set_finished() {
      fiber_transition_flag_ = eFinished;
    }

    void set_thread(Thread* thread);
    void set_fiber(Fiber* fiber);

    Thread* thread() {
      return thread_.get();
    }

    Fiber* fiber() {
      return fiber_.get();
    }

    void set_zombie(STATE);
    void set_zombie();

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

    bool limited_wait_for(std::function<bool ()> f) {
      bool status = false;

      // TODO: randomize wait interval
      for(int i = 0; i < cWaitLimit && !(status = f()); i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
      }

      return status;
    }

    void set_start_time();
    double run_time();

    void raise_stack_error(STATE);
    void validate_stack_size(STATE, size_t size);

    size_t stack_size() {
      return stack_size_;
    }

    void set_stack_bounds(size_t size) {
      void* stack_address;

      stack_size_ = size - stack_cushion_;
      stack_start_ = &stack_address;
    }

    bool check_stack(STATE, void* stack_address) {
      ssize_t stack_used =
        (reinterpret_cast<intptr_t>(stack_start_)
        - reinterpret_cast<intptr_t>(stack_address));

      if(stack_used < 0) stack_used = -stack_used;

      if(static_cast<size_t>(stack_used) > stack_size_) {
        raise_stack_error(state);
        return false;
      }

      return true;
    }

    bool push_call_frame(STATE, CallFrame* frame, CallFrame*& previous_frame);

    bool pop_call_frame(STATE, CallFrame* frame) {
      call_frame_ = frame;

      return !thread_interrupted_p(state);
    }

    bool thread_interrupted_p(STATE) {
      if(check_local_interrupts()) {
        return check_thread_raise_or_kill(state);
      }

      return false;
    }

    bool check_thread_raise_or_kill(STATE);

    // Do NOT de-duplicate
    void set_call_frame(CallFrame* frame) {
      call_frame_ = frame;
    }

    CallFrame* call_frame() {
      return call_frame_;
    }

    CallFrame* get_call_frame(ssize_t up=0);
    CallFrame* get_ruby_frame(ssize_t up=0);
    CallFrame* get_variables_frame(ssize_t up=0);
    CallFrame* get_scope_frame(ssize_t up=0);
    CallFrame* get_noncore_frame(STATE);
    CallFrame* get_filtered_frame(STATE, const std::regex& filter);

    bool scope_valid_p(VariableScope* scope);

    GlobalCache* global_cache() const {
      return shared.global_cache;
    }

    Globals& globals() {
      return shared.globals;
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
      return thread_step_;
    }

    void clear_thread_step() {
      clear_check_local_interrupts();
      thread_step_ = false;
    }

    void set_thread_step() {
      set_check_local_interrupts();
      thread_step_ = true;
    }

    bool check_local_interrupts() const {
      return check_local_interrupts_;
    }

    void clear_check_local_interrupts() {
      check_local_interrupts_ = false;
    }

    void set_check_local_interrupts() {
      check_local_interrupts_ = true;
    }

    bool interrupt_by_kill() const {
      return interrupt_by_kill_;
    }

    void clear_interrupt_by_kill() {
      interrupt_by_kill_ = false;
    }

    void set_interrupt_by_kill() {
      interrupt_by_kill_ = true;
    }

    Exception* interrupted_exception() const {
      return interrupted_exception_.get();
    }

    void clear_interrupted_exception() {
      interrupted_exception_.set(cNil);
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

    memory::VariableRootBuffers& current_root_buffers();

  public:
    static VM* current();

    static void discard(STATE, VM*);

  public:

    /* Prototypes */
    VM(uint32_t id, SharedState& shared, const char* name = NULL);
    ~VM();

    void bootstrap_class(STATE);
    void bootstrap_ontology(STATE);
    void bootstrap_symbol(STATE);

    void collect_maybe(STATE);

    native_int max_profile_entries() {
      return max_profile_entries_;
    }

    uint64_t profile_sample_count() {
      return profile_sample_count_;
    }

    CompiledCode** profile() {
      return profile_;
    }

    void update_profile(STATE);
    void sort_profile();

#define RBX_PROFILE_MAX_SHIFT     0xf
#define RBX_PROFILE_MAX_INTERVAL  0x1fff

    void set_profile_interval() {
      profile_interval_ = randombytes_random();
      profile_interval_ >>= (profile_interval_ & RBX_PROFILE_MAX_SHIFT);
      profile_interval_ &= RBX_PROFILE_MAX_INTERVAL;
      profile_counter_ = 0;
    }

    void checkpoint(STATE) {
      metrics().machine.checkpoints++;

      thread_nexus_->check_stop(state, this, [this, state]{
          metrics().machine.stops++;
          collect_maybe(state);
        });

      if(profile_counter_++ >= profile_interval_) {
        update_profile(state);
        set_profile_interval();
      }
    }

    void blocking_suspend(STATE, metrics::metric& counter);
    void sleeping_suspend(STATE, metrics::metric& counter);

    void blocking_phase(STATE) {
      thread_nexus_->blocking_phase(state, this);
    }

    void managed_phase(STATE) {
      thread_nexus_->managed_phase(state, this);
    }

    void unmanaged_phase(STATE) {
      thread_nexus_->unmanaged_phase(state, this);
    }

    void set_current_thread();

    void setup_errno(STATE, int num, const char* name, Class* sce, Module* ern);
    void bootstrap_exceptions(STATE);
    void initialize_fundamental_constants(STATE);
    void initialize_builtin_classes(STATE);
    void initialize_platform_data(STATE);
    Object* ruby_lib_version();

    TypeInfo* find_type(int type);

    static void init_ffi(STATE);

    void raise_from_errno(const char* reason);
    void raise_exception(Exception* exc);
    Exception* new_exception(Class* cls, const char* msg);
    Object* current_block();

    void set_const(const char* name, Object* val);
    void set_const(Module* mod, const char* name, Object* val);

    Object* path2class(const char* name);

    void print_backtrace();

    void wait_on_channel(Channel* channel);
    void wait_on_inflated_lock(Object* wait);
    void wait_on_custom_function(void (*func)(void*), void* data);
    void clear_waiter();
    bool wakeup(STATE);

    void reset_parked();

    void set_sleeping();
    void clear_sleeping();

    void interrupt_with_signal() {
      interrupt_with_signal_ = true;
    }

    void register_raise(STATE, Exception* exc);
    void register_kill(STATE);

    void gc_scan(memory::GarbageCollector* gc);
    void gc_verify(memory::GarbageCollector* gc);
  };
}

#endif
