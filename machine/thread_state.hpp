#ifndef RBX_STATE_HPP
#define RBX_STATE_HPP

#include "missing/time.h"

#include "diagnostics.hpp"
#include "globals.hpp"
#include "type_info.hpp"
#include "unwind_info.hpp"

#include "memory/root.hpp"
#include "memory/root_buffer.hpp"
#include "memory/thca.hpp"
#include "memory/variable_buffer.hpp"

#include "sodium/randombytes.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <regex>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <thread>
#include <vector>

namespace rubinius {
  struct CallFrame;

  class Fiber;
  class NativeMethodEnvironment;
  class VariableScope;
  class C_API;
  class Channel;
  class Class;
  class Console;
  class Environment;
  class Exception;
  class Object;
  class Machine;
  class MachineState;
  class Memory;
  class Park;
  class Profiler;
  class Signals;
  class String;
  class Symbol;
  class Threads;
  class UnwindState;

  namespace memory {
    class Collector;
  }

  enum MethodMissingReason {
    eNone,
    ePrivate,
    eProtected,
    eSuper,
    eVCall,
    eNormal
  };

  enum ConstantMissingReason {
    vFound,
    vPrivate,
    vNonExistent
  };

  class ThreadState {
  public:
    const static uint64_t cLockLimit = 5000000000;

    enum Kind {
      eThread,
      eFiber,
      eSystem
    };

    enum Phase {
      eManaged    = 0x01,
      eUnmanaged  = 0x81,
      eWaiting    = 0x82,
    };

    const static int cYieldingPhase = 0x80;

    enum ThreadStatus {
      eNoStatus = 0,
      eRun,
      eSleep,
      eDead,
      eException
    };

    // TODO: Thread
    enum FiberTransition {
      eSuspending,
      eSuspended,
      eResuming,
      eRunning,
      eCanceled,
      eFinished
    };

  private:
    jmp_buf thread_unwind_;
    bool thread_unwinding_;
    memory::Roots roots_;
    std::string name_;
    memory::VariableRootBuffers variable_root_buffers_;
    memory::RootBuffers root_buffers_;
    Kind kind_;
    diagnostics::MachineMetrics* metrics_;

  protected:
    pthread_t os_thread_;
    uint32_t id_;

    friend class ThreadState;

  private:
    static const int cWaitLimit = 100;

    Machine* _machine_;

    UnwindInfoSet unwinds_;

    CallFrame* call_frame_;
    memory::THCA* thca_;

    int8_t* stack_start_;
    int8_t* stack_barrier_start_;
    int8_t* stack_barrier_end_;

    size_t stack_size_;
    size_t stack_cushion_;
    ssize_t stack_probe_;

    bool interrupt_with_signal_;
    bool interrupt_by_kill_;

    std::atomic<bool> should_wakeup_;
    std::atomic<ThreadStatus> thread_status_;

    std::mutex lock_;
    std::mutex thread_lock_;
    std::mutex sleep_lock_;
    std::condition_variable sleep_cond_;
    std::mutex join_lock_;
    std::condition_variable join_cond_;
    std::mutex fiber_mutex_;

    std::mutex fiber_wait_mutex_;
    std::condition_variable fiber_wait_condition_;

    std::atomic<FiberTransition> fiber_transition_flag_;

    MethodMissingReason method_missing_reason_;
    ConstantMissingReason constant_missing_reason_;

    bool main_thread_;

    std::atomic<Phase> thread_phase_;

    uint64_t sample_interval_;
    uint64_t sample_counter_;

    bool checkpoint_;

    diagnostics::metric checkpoints_;
    diagnostics::metric stops_;

  public:
    /* Data members */
    Channel* waiting_channel_;
    Exception* interrupted_exception_;
    /// The Thread object for this VM state
    Thread* thread_;
    Fiber* fiber_;

    /// Object that waits for inflation
    Object* waiting_object_;

    uint64_t start_time_;

    NativeMethodEnvironment* native_method_environment;

    void (*custom_wakeup_)(void*);
    void* custom_wakeup_data_;

    UnwindState* unwind_state_;

  public:
    ThreadState(uint32_t id, Machine* m, const char* name = nullptr);
    ~ThreadState();

    MachineState* const machine_state();

    Machine* const machine() {
      return _machine_;
    }

    jmp_buf& get_thread_unwind() {
      return thread_unwind_;
    }

    bool thread_unwinding_p() {
      return thread_unwinding_;
    }

    Configuration* const configuration();
    Environment* const environment();
    Threads* const threads();
    Diagnostics* const diagnostics();
    memory::Collector* const collector();
    Signals* const signals();
    Memory* const memory();
    C_API* const c_api();
    Profiler* const profiler();
    Console* const console();

    // -*-*-*
    static ThreadState* current();

    memory::Roots& roots() {
      return roots_;
    }

    memory::VariableRootBuffers& variable_root_buffers() {
      return variable_root_buffers_;
    }

    memory::RootBuffers& root_buffers() {
      return root_buffers_;
    }

    const char* kind_name() const {
      switch(kind_) {
        case eThread:
          return "Thread";
        case eFiber:
          return "Fiber";
        case eSystem:
          return "MachineThread";
      }

      /* GCC cannot determine that the above switch covers the enum and hence
       * every exit from this function is covered.
       */
      return "unknown kind";
    }

    Kind kind() const {
      return kind_;
    }

    void set_kind(Kind kind) {
      kind_ = kind;
    }

    std::string name() const {
      return name_;
    }

    void set_name(STATE, const char* name);

    pthread_t& os_thread() {
      return os_thread_;
    }

    diagnostics::MachineMetrics* metrics() {
      return metrics_;
    }

    UnwindInfoSet& unwinds() {
      return unwinds_;
    }

    uint32_t thread_id() const {
      return id_;
    }

    const char* phase_name();

    Phase thread_phase() {
      return thread_phase_.load(std::memory_order_acquire);
    }

    void set_thread_phase(Phase thread_phase) {
      thread_phase_.store(thread_phase, std::memory_order_release);
    }

    bool wakeup_p() {
      return should_wakeup_;
    }

    void unset_wakeup() {
      should_wakeup_ = false;
    }

    void set_wakeup() {
      should_wakeup_ = true;
    }

    std::mutex& thread_lock() {
      return thread_lock_;
    }

    std::mutex& sleep_lock() {
      return sleep_lock_;
    }

    std::condition_variable& sleep_cond() {
      return sleep_cond_;
    }

    std::mutex& join_lock() {
      return join_lock_;
    }

    std::condition_variable& join_cond() {
      return join_cond_;
    }

    std::mutex& fiber_mutex() {
      return fiber_mutex_;
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
      return thread_;
    }

    Fiber* fiber() {
      return fiber_;
    }

    ThreadStatus thread_status() {
      return thread_status_;
    }

    void set_thread_run() {
      thread_status_ = eRun;
    }

    void set_thread_sleep() {
      thread_status_ = eSleep;
    }

    void set_thread_dead() {
      thread_status_ = eDead;
    }

    void set_thread_exception() {
      thread_status_ = eException;
    }

    bool sleeping_p() {
      return thread_status_ == eSleep;
    }

    bool zombie_p() {
      return thread_status_ == eDead || thread_status_ == eException;
    }

    void set_main_thread() {
      main_thread_ = true;
    }

    bool main_thread_p() {
      return main_thread_;
    }

    Object* allocate_object(STATE, intptr_t bytes, object_type type) {
      return thca_->allocate(state, bytes, type);
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
      int8_t stack_address;

      stack_size_ = size - stack_cushion_;
      stack_start_ = &stack_address;

      // Determine the direction of the stack
      set_stack_barrier();
    }

    void set_stack_barrier() {
      int8_t barrier;

      if(stack_start_ - &barrier < 0) {
        // barrier = reinterpret_cast<int8_t*>(stack_start_ + stack_size_ - 2 * stack_cushion_);
        stack_probe_ = stack_cushion_ / 2;
        stack_barrier_start_ = stack_start_ + stack_size_ - 2 * stack_cushion_;
        stack_barrier_end_ = stack_barrier_start_ + stack_cushion_;
      } else {
        // barrier = reinterpret_cast<void*>(ss - stack_size_ + stack_cushion_);
        stack_probe_ = -(stack_cushion_ / 2);
        stack_barrier_end_ = stack_start_ - stack_size_ + stack_cushion_;
        stack_barrier_start_ = stack_barrier_end_ - stack_cushion_;
      }
    }

    bool stack_limit_p(void* address) {
      int8_t* probe = reinterpret_cast<int8_t*>(address) + stack_probe_;

      return probe > stack_barrier_start_ && probe <= stack_barrier_end_;
    }

    bool check_stack(STATE, void* address) {
      if(stack_limit_p(address)) {
        raise_stack_error(state);
        return false;
      }

      return true;
    }

    void set_previous_frame(CallFrame* frame);

    bool push_call_frame(STATE, CallFrame* frame) {
      if(!check_stack(state, frame)) return false;

      set_previous_frame(frame);
      call_frame_ = frame;

      return true;
    }

    bool pop_call_frame(STATE, CallFrame* frame) {
      call_frame_ = frame;

      return !thread_interrupted_p();
    }

    bool thread_interrupted_p() {
      return check_thread_raise_or_kill(this);
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

    Globals& globals();

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

    void after_fork_child();

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
      return interrupted_exception_;
    }

    void clear_interrupted_exception();

    memory::VariableRootBuffers& current_root_buffers();

  public:
    void discard();

    void bootstrap_class(STATE);
    void bootstrap_ontology(STATE);
    void bootstrap_symbol(STATE);

    void sample(STATE);

#define RBX_PROFILE_MAX_SHIFT     0xf
#define RBX_PROFILE_MAX_INTERVAL  0x1fff

    void set_sample_interval() {
      sample_interval_ = randombytes_random();
      sample_interval_ >>= (sample_interval_ & RBX_PROFILE_MAX_SHIFT);
      sample_interval_ &= RBX_PROFILE_MAX_INTERVAL;
      sample_counter_ = 0;
    }

    void set_checkpoint() {
      checkpoint_ = true;
    }

    void checkpoint(STATE) {
      if(!checkpoint_) return;

      ++checkpoints_;

      if(check_stop()) {
        ++stops_;
        checkpoint_ = false;
      }

      // TODO: profiler
      if(sample_counter_++ >= sample_interval_) {
        sample(state);
        set_sample_interval();
      }
    }

#define SET_THREAD_UNWIND(ts) /*ep.file = __FILE__; ep.line = __LINE__; */ _setjmp(ts->get_thread_unwind())

    void halt_thread();

    void managed_phase();
    void unmanaged_phase();

    void set_current_thread();

    void setup_errno(STATE, int num, const char* name, Class* sce, Module* ern);
    void bootstrap_exceptions(STATE);
    void initialize_fundamental_constants(STATE);
    void initialize_builtin_classes(STATE);
    void initialize_platform_data(STATE);
    Object* ruby_lib_version();

    static void init_ffi(STATE);

    void raise_from_errno(const char* reason);
    void raise_exception(Exception* exc);
    Exception* new_exception(Class* cls, const char* msg);
    Object* current_block();

    Object* path2class(STATE, const char* name);

    void wait_on_channel(Channel* channel);
    void wait_on_custom_function(STATE, void (*func)(void*), void* data);
    void clear_waiter();

    void sleep(Object* duration);
    bool wakeup();

    void interrupt_with_signal() {
      interrupt_with_signal_ = true;
    }

    void register_raise(STATE, Exception* exc);
    void register_kill(STATE);

    void visit_objects(STATE, std::function<void (STATE, Object**)> f);
    void trace_objects(STATE, std::function<void (STATE, Object**)> f);

    // -*-*-*

    Symbol* const symbol(const char* str);
    Symbol* const symbol(const char* str, size_t len);
    Symbol* const symbol(std::string str);
    Symbol* const symbol(String* str);

    const uint32_t hash_seed();

    UnwindState* unwind_state();

    void raise_stack_error();

    // TODO ThreadNexus
    bool stop_p();
    bool set_stop();
    void unset_stop();
    bool halt_p();
    void set_halt();

    bool try_managed_phase();
    void waiting_phase();

    void set_managed();

    bool valid_thread_p(unsigned int thread_id);

#ifdef RBX_GC_STACK_CHECK
    void check_stack(STATE);
#endif

    bool yielding_p();

    void yield();

    uint64_t wait();
    void wait_for_all();

    bool lock_owned_p();

    bool try_lock();
    bool try_lock_wait();

    bool check_stop() {
      if(!can_stop_p()) return false;

      while(stop_p()) {
        yield();

        return true;
      }

      return false;
    }

    bool can_stop_p();

    void stop() {
      while(set_stop()) {
        if(try_lock_wait()) {
          wait_for_all();

          return;
        }
      }
    }

    void halt() {
      set_halt();
      stop();
      unset_stop();
      unlock();
    }

    void lock(std::function<void ()> process) {
      lock();
      process();
      unlock();
    }

    void lock() {
      try_lock_wait();
    }

    bool try_lock(std::function<void ()> process) {
      if(try_lock()) {
        process();
        unlock();
        return true;
      } else {
        return false;
      }
    }

    void unlock();

    void detect_deadlock(uint64_t nanoseconds);
    void detect_deadlock(uint64_t nanoseconds, ThreadState* thread_state);

    // TODO ThreadNexus end
  };
}

#endif
