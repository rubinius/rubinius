#include "call_frame.hpp"
#include "environment.hpp"
#include "machine.hpp"
#include "helpers.hpp"
#include "machine_code.hpp"
#include "memory.hpp"
#include "signals.hpp"
#include "thread_phase.hpp"
#include "thread_state.hpp"

#include "class/compiled_code.hpp"
#include "class/exception.hpp"
#include "class/location.hpp"
#include "class/array.hpp"
#include "class/fiber.hpp"
#include "class/fixnum.hpp"
#include "class/float.hpp"
#include "class/unwind_state.hpp"

#include "diagnostics/machine.hpp"

#include <cxxabi.h>

#include <chrono>
#include <thread>
#include <sstream>
#include <signal.h>

// Reset macros since we're inside state
#undef G
#undef GO
#define G(whatever) globals().whatever.get()
#define GO(whatever) globals().whatever

namespace rubinius {
  static thread_local ThreadState* _current_thread = nullptr;

  ThreadState::ThreadState(uint32_t id, Machine* m, const char* name)
    : thread_unwinding_(false)
    , kind_(eThread)
    , metrics_(new diagnostics::MachineMetrics())
    , os_thread_(0)
    , id_(id)
    , _machine_(m)
    , call_frame_(nullptr)
    , thca_(new memory::OpenTHCA)
    , stack_start_(0)
    , stack_barrier_start_(0)
    , stack_barrier_end_(0)
    , stack_size_(0)
    , stack_cushion_(m->configuration()->machine_stack_cushion.value)
    , stack_probe_(0)
    , interrupt_with_signal_(false)
    , interrupt_by_kill_(false)
    , should_wakeup_(true)
    , thread_status_(eRun)
    , lock_()
    , thread_lock_()
    , sleep_lock_()
    , sleep_cond_()
    , join_lock_()
    , join_cond_()
    , fiber_mutex_()
    , fiber_wait_mutex_()
    , fiber_wait_condition_()
    , fiber_transition_flag_(eSuspending)
    , method_missing_reason_(eNone)
    , constant_missing_reason_(vFound)
    , main_thread_(false)
    , thread_phase_(eUnmanaged)
    , sample_interval_(0)
    , sample_counter_(0)
    , checkpoints_(0)
    , stops_(0)
    , waiting_channel_(nil<Channel>())
    , interrupted_exception_(nil<Exception>())
    , thread_(nil<Thread>())
    , fiber_(nil<Fiber>())
    , start_time_(0)
    , native_method_environment(nullptr)
    , custom_wakeup_(nullptr)
    , custom_wakeup_data_(nullptr)
    , unwind_state_(nullptr)
  {
    if(name) {
      name_.assign(name);
    } else {
      std::ostringstream thread_name;
      thread_name << "ruby." << id_;
      name_.assign(thread_name.str());
    }

    set_sample_interval();
  }

  ThreadState::~ThreadState() {
    logger::info("%s: checkpoints: %ld, stops: %ld",
        name().c_str(), checkpoints_, stops_);

    if(thca_) {
      delete thca_;
      thca_ = nullptr;
    }

    if(metrics_) {
      delete metrics_;
      metrics_ = nullptr;
    }
  }

  void ThreadState::raise_exception(Exception* exc) {
    unwind_state()->raise_exception(exc);
  }

  void ThreadState::raise_stack_error() {
    Class* stack_error = globals().stack_error.get();
    Exception* exc = memory()->new_object<Exception>(this, stack_error);
    exc->locations(this, Location::from_call_stack(this));
    unwind_state()->raise_exception(exc);
  }

  const uint32_t ThreadState::hash_seed() {
    return machine()->machine_state()->hash_seed();
  }

  MachineState* const ThreadState::machine_state() {
    return machine()->machine_state();
  }

  Threads* const ThreadState::threads() {
    return machine()->threads();
  }

  Configuration* const ThreadState::configuration() {
    return machine()->configuration();
  }

  Environment* const ThreadState::environment() {
    return machine()->environment();
  }

  Diagnostics* const ThreadState::diagnostics() {
    return machine()->diagnostics();
  }

  memory::Collector* const ThreadState::collector() {
    return machine()->collector();
  }

  Signals* const ThreadState::signals() {
    return machine()->signals();
  }

  Memory* const ThreadState::memory() {
    return machine()->memory();
  }

  C_API* const ThreadState::c_api() {
    return machine()->c_api();
  }

  Profiler* const ThreadState::profiler() {
    return machine()->profiler();
  }

  Console* const ThreadState::console() {
    return machine()->console();
  }

  Globals& ThreadState::globals() {
    return memory()->globals;
  }

  Symbol* const ThreadState::symbol(const char* str) {
    return memory()->symbols.lookup(this, str, strlen(str));
  }

  Symbol* const ThreadState::symbol(const char* str, size_t len) {
    return memory()->symbols.lookup(this, str, len);
  }

  Symbol* const ThreadState::symbol(std::string str) {
    return memory()->symbols.lookup(this, str);
  }

  Symbol* const ThreadState::symbol(String* str) {
    return memory()->symbols.lookup(this, str);
  }

  // TODO ThreadNexus
  bool ThreadState::stop_p() {
    return machine()->stop().load(std::memory_order_acquire);
  }

  bool ThreadState::set_stop() {
    machine()->stop().store(true, std::memory_order_release);

    return stop_p();
  }

  void ThreadState::unset_stop() {
    machine()->stop().store(false, std::memory_order_release);
  }

  bool ThreadState::halt_p() {
    return machine()->halting().load(std::memory_order_acquire) != 0;
  }

  void ThreadState::set_halt() {
    if(machine()->halting() && machine()->halting() == thread_id()) {
      std::ostringstream msg;

      msg << "halting mutex is already locked: id: " << thread_id();

      Exception::raise_assertion_error(this, msg.str().c_str());
    }

    machine()->halting().store(thread_id(), std::memory_order_release);
  }

  void ThreadState::managed_phase() {
    if(machine()->halting() && machine()->halting() != thread_id()) {
      halt_thread();
    }

    if(can_stop_p()) {
      lock([&]{ set_thread_phase(eManaged); });
    } else {
      // We already own the lock
      set_thread_phase(eManaged);
    }
  }


  bool ThreadState::try_managed_phase() {
    if(machine()->halting() && machine()->halting() != thread_id()) {
      halt_thread();
    }

    return try_lock([&]{ set_thread_phase(eManaged); });
  }

  void ThreadState::unmanaged_phase() {
    set_thread_phase(eUnmanaged);
  }

  void ThreadState::waiting_phase() {
    if(!can_stop_p()) {
      std::ostringstream msg;

      msg << "waiting while holding process-critical lock: id: " << thread_id();

      Exception::raise_assertion_error(this, msg.str().c_str());
    }

    set_thread_phase(eWaiting);
  }

  // Only to be used when holding the ThreadNexus lock.
  void ThreadState::set_managed() {
    set_thread_phase(eManaged);
  }

  bool ThreadState::can_stop_p() {
    return machine()->threads_lock() != thread_id();
  }

  void ThreadState::yield() {
    while(stop_p()) {
      waiting_phase();

#ifdef RBX_GC_STACK_CHECK
      check_stack();
#endif

      if(halt_p()) {
        halt_thread();
      } else {
        std::unique_lock<std::mutex> lock(machine()->waiting_mutex());
        machine()->waiting_condition().wait(lock, [this]{ return !stop_p(); });
      }

      managed_phase();
    }
  }

  void ThreadState::unlock() {
    if(can_stop_p()) {
      std::ostringstream msg;

      msg << "process-critical lock being unlocked by the wrong Thread: id: "
          << thread_id();

      Exception::raise_assertion_error(this, msg.str().c_str());
    }

    machine()->waiting_condition().notify_all();
    machine()->threads_lock().store(0);
  }

  bool ThreadState::yielding_p() {
    int phase = static_cast<int>(thread_phase());

    return (phase & cYieldingPhase) == cYieldingPhase;
  }

  const char* ThreadState::phase_name() {
    switch(thread_phase()) {
      case eManaged:
        return "eManaged";
      case eUnmanaged:
        return "eUnmanaged";
      case eWaiting:
        return "eWaiting";
    }

    return "cUnknown";
  }

  static void list_threads(STATE, logger::PrintFunction function) {
    state->threads()->each(state, [function](STATE, ThreadState* thread_state) {
          function("thread %d: %s, %s",
              thread_state->thread_id(),
              thread_state->name().c_str(),
              thread_state->phase_name());
        });
  }

  void ThreadState::detect_deadlock(uint64_t nanoseconds, ThreadState* thread_state) {
    if(nanoseconds > cLockLimit) {
      logger::error("thread nexus: thread will not yield: %s, %s",
          thread_state->name().c_str(), thread_state->phase_name());

      list_threads(this, logger::error);

      std::ostringstream msg;
      msg << "thread will not yield: " << thread_state->name().c_str() << phase_name();

      Exception::raise_deadlock_error(this, msg.str().c_str());
    }
  }

  void ThreadState::detect_deadlock(uint64_t nanoseconds) {
    if(nanoseconds > cLockLimit) {
      logger::error("thread nexus: unable to lock, possible deadlock");

      list_threads(this, logger::error);

      Exception::raise_deadlock_error(this,
          "thread nexus: unable to lock, possible deadlock");
    }
  }

  bool ThreadState::valid_thread_p(unsigned int thread_id) {
    bool valid = false;

    threads()->each(this, [&](STATE, ThreadState* thr) {
        if(thread_id == thr->thread_id()) valid = true;
      });

    return valid;
  }

  uint64_t ThreadState::wait() {
    static int i = 0;
    static int delay[] = {
      133, 464, 254, 306, 549, 287, 358, 638, 496, 81,
      472, 288, 131, 31, 435, 258, 221, 73, 537, 854
    };
    static int modulo = sizeof(delay) / sizeof(int);

    uint64_t ns = delay[i++ % modulo];

    std::this_thread::sleep_for(std::chrono::nanoseconds(ns));

    return ns;
  }

  void ThreadState::wait_for_all() {
    uint64_t limit = 0;

    set_managed();

    threads()->each(this, [&](STATE, ThreadState* thread_state) {
        if(!(state == thread_state || thread_state->yielding_p())) {
          while(true) {
            if(thread_state->yielding_p()) {
              break;
            }

            limit += wait();

            detect_deadlock(limit, thread_state);
          }
        }
      });
  }

  bool ThreadState::lock_owned_p() {
    return machine()->threads_lock() == thread_id();
  }

  bool ThreadState::try_lock() {
    uint32_t id = 0;

    return machine()->threads_lock().compare_exchange_strong(id, thread_id());
  }

  bool ThreadState::try_lock_wait() {
    uint64_t limit = 0;

    while(!try_lock()) {
      if(collector()->collect_requested_p()) {
        yield();
      }

      limit += wait();

      detect_deadlock(limit);
    }

    return true;
  }

  static char* demangle(char* symbol, char* result, size_t size) {
    const char* pos = strstr(symbol, " _Z");

    if(pos) {
      size_t sz = 0;
      char *cpp_name = 0;
      char* name = strdup(pos + 1);
      char* end = strstr(name, " + ");
      *end = 0;

      int status;
      if((cpp_name = abi::__cxa_demangle(name, cpp_name, &sz, &status))) {
        if(!status) {
          snprintf(result, size, "%.*s %s %s", int(pos - symbol), symbol, cpp_name, ++end);
        }
        free(cpp_name);
      }

      free(name);
    } else {
      strcpy(result, symbol);
    }

    return result;
  }

#ifdef RBX_GC_STACK_CHECK
#define RBX_ABORT_CALLSTACK_SIZE    128
#define RBX_ABORT_SYMBOL_SIZE       512

  void ThreadState::check_stack(STATE, ThreadState* thread_state) {
    void* callstack[RBX_ABORT_CALLSTACK_SIZE];
    char symbol[RBX_ABORT_SYMBOL_SIZE];

    int i, frames = backtrace(callstack, RBX_ABORT_CALLSTACK_SIZE);
    char** symbols = backtrace_symbols(callstack, frames);

    logger::debug("Backtrace for %s: %s",
        thread_state->kind_name(), thread_state->name().c_str());
    for(i = 0; i < frames; i++) {
      logger::debug("%s", demangle(symbols[i], symbol, RBX_ABORT_SYMBOL_SIZE));
    }

    free(symbols);
  }
#endif
  // TODO ThreadNexus end

  // -*-*-*-
  void ThreadState::set_name(STATE, const char* name) {
#ifdef HAVE_PTHREAD_SETNAME
      pthread_setname_np(name);
#endif

    name_.assign(name);
  }

  ThreadState* ThreadState::current() {
    return _current_thread;
  }

  void ThreadState::set_current_thread() {
#ifdef HAVE_PTHREAD_SETNAME
      pthread_setname_np(name().c_str());
#endif

    _current_thread = this;
  }

  void ThreadState::clear_interrupted_exception() {
    interrupted_exception_ = nil<Exception>();
  }

  void ThreadState::set_thread(Thread* thread) {
    thread_ = thread;
  }

  void ThreadState::set_fiber(Fiber* fiber) {
    fiber_ = fiber;
  }

  void ThreadState::set_start_time() {
    start_time_ = get_current_time();
  }

  double ThreadState::run_time() {
    return timer::time_elapsed_seconds(start_time_);
  }

  void ThreadState::set_previous_frame(CallFrame* frame) {
    frame->previous = call_frame_;
  }

  void ThreadState::raise_stack_error(STATE) {
    state->raise_stack_error();
  }

  void ThreadState::validate_stack_size(STATE, size_t size) {
    if(stack_cushion_ > size) {
      Exception::raise_runtime_error(state, "requested stack size is invalid");
    }
  }

  UnwindState* ThreadState::unwind_state() {
    if(!unwind_state_) {
      unwind_state_ = UnwindState::create(this);
    }

    return unwind_state_;
  }

  bool ThreadState::check_thread_raise_or_kill(STATE) {
    Exception* exc = interrupted_exception();

    if(!exc->nil_p()) {
      clear_interrupted_exception();

      // Only write the locations if there are none.
      if(exc->locations()->nil_p() || exc->locations()->size() == 0) {
        exc->locations(state, Location::from_call_stack(state));
      }

      unwind_state()->raise_exception(exc);

      return true;
    }

    if(interrupt_by_kill()) {
      if(state->thread()->current_fiber()->root_p()) {
        clear_interrupt_by_kill();
      }

      unwind_state()->raise_thread_kill();

      return true;
    }

    return false;
  }

  CallFrame* ThreadState::get_call_frame(ssize_t up) {
    CallFrame* frame = call_frame_;

    while(frame && up-- > 0) {
      frame = frame->previous;
    }

    return frame;
  }

  CallFrame* ThreadState::get_ruby_frame(ssize_t up) {
    CallFrame* frame = call_frame_;

    while(frame && up-- > 0) {
      frame = frame->previous;
    }

    while(frame) {
      if(!frame->native_method_p()) return frame;
      frame = frame->previous;
    }

    return NULL;
  }

  CallFrame* ThreadState::get_noncore_frame(STATE) {
    for(CallFrame* frame = call_frame_; frame; frame = frame->previous) {
      if(frame->native_method_p()) continue;

      CompiledCode* code = frame->compiled_code;
      if(code && !code->nil_p()) {
        if(!code->core_method(state)) return frame;
      }
    }

    return NULL;
  }

  CallFrame* ThreadState::get_filtered_frame(STATE, const std::regex& filter) {
    for(CallFrame* frame = call_frame_; frame; frame = frame->previous) {
      if(frame->native_method_p()) continue;

      CompiledCode* code = frame->compiled_code;
      if(code && !code->nil_p() && !code->file()->nil_p()) {
        if(!std::regex_match(code->file()->cpp_str(state), filter)) {
          return frame;
        }
      }
    }

    return NULL;
  }

  CallFrame* ThreadState::get_variables_frame(ssize_t up) {
    CallFrame* frame = call_frame_;

    while(frame && up-- > 0) {
      frame = frame->previous;
    }

    while(frame) {
      if(!frame->is_inline_block()
          && !frame->native_method_p()
          && frame->scope)
      {
        return frame;
      }

      frame = frame->previous;
    }

    return NULL;
  }

  CallFrame* ThreadState::get_scope_frame(ssize_t up) {
    CallFrame* frame = call_frame_;

    while(frame && up-- > 0) {
      frame = frame->previous;
    }

    while(frame) {
      if(frame->scope) return frame;
      frame = frame->previous;
    }

    return NULL;
  }

  bool ThreadState::scope_valid_p(VariableScope* scope) {
    CallFrame* frame = call_frame_;

    while(frame) {
      if(frame->scope && frame->scope->on_heap() == scope) {
        return true;
      }

      frame = frame->previous;
    }

    return false;
  }

  void ThreadState::sample(STATE) {
    timer::StopWatch<timer::nanoseconds> timer(metrics()->sample_ns);

    metrics()->samples++;

    CallFrame* frame = state->call_frame();

    while(frame) {
      // TODO: add counters to native method frames
      if(frame->compiled_code) {
        frame->compiled_code->machine_code()->sample_count++;
      }

      frame = frame->previous;
    }
  }

  void ThreadState::discard() {
    // TODO: Diagnostics
    metrics()->threads_destroyed++;

    /* It is undefined behavior to destroy a condition variable or mutex that
     * is being waited on / locked by a thread. By using placement new, we
     * overwrite that data in the mutexes and condition variables.
     */
    new(&lock_) std::mutex;
    new(&thread_lock_) std::mutex;
    new(&sleep_lock_) std::mutex;
    new(&sleep_cond_) std::condition_variable;
    new(&join_lock_) std::mutex;
    new(&join_cond_) std::condition_variable;
    new(&fiber_mutex_) std::mutex;
    new(&fiber_wait_mutex_) std::mutex;
    new(&fiber_wait_condition_) std::condition_variable;

    delete this;
  }

  void ThreadState::halt_thread() {
    thread_unwinding_ = true;
    _longjmp(thread_unwind_, 1);
  }

  void type_assert(STATE, Object* obj, object_type type, const char* reason) {
    if((obj->reference_p() && obj->type_id() != type)
        || (type == FixnumType && !obj->fixnum_p())) {
      std::ostringstream msg;
      msg << reason << ": " << obj->to_string(state, true);
      Exception::raise_type_error(state, type, obj, msg.str().c_str());
    }
  }

  void ThreadState::after_fork_child() {
    set_main_thread();

    set_start_time();

    // TODO: Remove need for root_vm.
    environment()->set_root_vm(this);
  }

  Object* ThreadState::path2class(STATE, const char* path) {
    Module* mod = state->memory()->globals.object.get();

    char* copy = strdup(path);
    char* cur = copy;

    for(;;) {
      char* pos = strstr(cur, "::");
      if(pos) *pos = 0;

      Object* obj = mod->get_const(state, state->symbol(cur));

      if(pos) {
        if(Module* m = try_as<Module>(obj)) {
          mod = m;
        } else {
          free(copy);
          return cNil;
        }
      } else {
        free(copy);
        return obj;
      }

      cur = pos + 2;
    }
  }

  static void suspend_thread() {
    static int i = 0;
    static int delay[] = {
      45, 17, 38, 31, 10, 40, 13, 37, 16, 37, 1, 20, 23, 43, 38, 4, 2, 26, 25, 5
    };
    static int modulo = sizeof(delay) / sizeof(int);

    std::chrono::microseconds n(delay[i++ % modulo]);
    std::this_thread::sleep_for(n);
  }

  void ThreadState::sleep(Object* duration) {
    std::unique_lock<std::mutex> lock(sleep_lock_);

    unset_wakeup();
    set_thread_sleep();

    double seconds = 0;

    if(Fixnum* fix = try_as<Fixnum>(duration)) {
      seconds = fix->to_native();
    } else if(Float* flt = try_as<Float>(duration)) {
      seconds = flt->value();
    } else if(duration->nil_p()) {
      seconds = 0.01;
    }

    std::chrono::duration<double> pause(seconds);

    {
      UnmanagedPhase guard(this);

      while(!wakeup_p() && !thread_interrupted_p()) {
        auto status = sleep_cond_.wait_for(lock, pause);

        if(status == std::cv_status::timeout && !duration->nil_p()) {
          // We were woken because duration elapsed.
          break;
        }
      }
    }

    set_thread_run();
  }

  bool ThreadState::wakeup() {
    std::lock_guard<std::mutex> guard(lock_);

    if(sleeping_p()) {
      set_wakeup();

      std::unique_lock<std::mutex> lock(sleep_lock_);
      sleep_cond_.notify_one();
    } else if(interrupt_with_signal_ && os_thread_) {
      pthread_kill(os_thread_, SIGVTALRM);
    } else if(custom_wakeup_) {
      (*custom_wakeup_)(custom_wakeup_data_);
    }

    return true;
  }

  void ThreadState::clear_waiter() {
    std::lock_guard<std::mutex> guard(lock_);

    interrupt_with_signal_ = false;
    custom_wakeup_ = 0;
    custom_wakeup_data_ = 0;
  }

  void ThreadState::wait_on_custom_function(STATE, void (*func)(void*), void* data) {
    std::lock_guard<std::mutex> guard(lock_);

    custom_wakeup_ = func;
    custom_wakeup_data_ = data;
  }

  void ThreadState::register_raise(STATE, Exception* exc) {
    std::lock_guard<std::mutex> guard(lock_);
    interrupted_exception_ = exc;
  }

  void ThreadState::register_kill(STATE) {
    std::lock_guard<std::mutex> guard(lock_);
    set_interrupt_by_kill();
  }

  memory::VariableRootBuffers& ThreadState::current_root_buffers() {
    return variable_root_buffers();
  }

  void ThreadState::visit_objects(STATE, std::function<void (STATE, Object**)> f) {
    CallFrame* frame = call_frame_;

    while(frame) {
      f(state, reinterpret_cast<Object**>(&frame->lexical_scope_));
      f(state, reinterpret_cast<Object**>(&frame->compiled_code));

      if(frame->compiled_code) {
        intptr_t stack_size = frame->compiled_code->stack_size()->to_native();
        for(intptr_t i = 0; i < stack_size; i++) {
          f(state, &frame->stk[i]);
        }
      }

      f(state, reinterpret_cast<Object**>(&frame->top_scope_));

      BlockEnvironment* be = frame->block_env();
      f(state, reinterpret_cast<Object**>(&be));
      frame->set_block_env(be);

      Arguments* args = frame->arguments;

      if(!frame->inline_method_p() && args) {
        Object* recv = args->recv();
        f(state, &recv);
        args->set_recv(recv);

        Object* block = args->block();
        f(state, &block);
        args->set_block(block);

        if(Tuple* tup = args->argument_container()) {
          f(state, reinterpret_cast<Object**>(&tup));
          args->update_argument_container(tup);
        } else {
          Object** ary = args->arguments();
          for(uint32_t i = 0; i < args->total(); i++) {
            f(state, &ary[i]);
          }
        }
      }

      if(frame->scope && frame->compiled_code) {
        StackVariables* scope = frame->scope;

        f(state, reinterpret_cast<Object**>(&scope->self_));
        f(state, reinterpret_cast<Object**>(&scope->block_));
        f(state, reinterpret_cast<Object**>(&scope->module_));

        int locals = frame->compiled_code->machine_code()->number_of_locals;
        for(int i = 0; i < locals; i++) {
          Object* local = scope->get_local(i);
          f(state, &local);
          scope->set_local(i, local);
        }

        f(state, reinterpret_cast<Object**>(&scope->last_match_));
        f(state, reinterpret_cast<Object**>(&scope->parent_));
        f(state, reinterpret_cast<Object**>(&scope->on_heap_));
      }

      frame = frame->previous;
    }
  }

  void ThreadState::trace_objects(STATE, std::function<void (STATE, Object**)> f) {
    metrics()->checkpoints = checkpoints_;
    metrics()->stops = stops_;

    f(state, reinterpret_cast<Object**>(&waiting_channel_));
    f(state, reinterpret_cast<Object**>(&interrupted_exception_));
    f(state, reinterpret_cast<Object**>(&thread_));
    f(state, reinterpret_cast<Object**>(&fiber_));
    f(state, reinterpret_cast<Object**>(&unwind_state_));

    thca_->collect(state);

    CallFrame* frame = call_frame_;

    while(frame) {
      f(state, reinterpret_cast<Object**>(&frame->return_value));

      f(state, reinterpret_cast<Object**>(&frame->lexical_scope_));

      f(state, reinterpret_cast<Object**>(&frame->compiled_code));

      if(frame->compiled_code) {
        intptr_t stack_size = frame->compiled_code->stack_size()->to_native();
        for(intptr_t i = 0; i < stack_size; i++) {
          f(state, &frame->stk[i]);
        }
      }

      if(frame->multiple_scopes_p() && frame->top_scope_) {
        f(state, reinterpret_cast<Object**>(&frame->top_scope_));
      }

      if(BlockEnvironment* env = frame->block_env()) {
        f(state, reinterpret_cast<Object**>(&env));
        frame->set_block_env(env);
      }

      Arguments* args = frame->arguments;

      if(!frame->inline_method_p() && args) {
        Object* recv = args->recv();
        f(state, &recv);
        args->set_recv(recv);

        Object* block = args->block();
        f(state, &block);
        args->set_block(block);

        if(Tuple* tup = args->argument_container()) {
          f(state, reinterpret_cast<Object**>(&tup));
          args->update_argument_container(tup);
        } else {
          Object** ary = args->arguments();
          for(uint32_t i = 0; i < args->total(); i++) {
            f(state, &ary[i]);
          }
        }
      }

      if(frame->scope && frame->compiled_code) {
        // saw_variable_scope(frame, displace(frame->scope, offset));
        StackVariables* scope = frame->scope;

        f(state, reinterpret_cast<Object**>(&scope->self_));
        f(state, reinterpret_cast<Object**>(&scope->block_));
        f(state, reinterpret_cast<Object**>(&scope->module_));

        int locals = frame->compiled_code->machine_code()->number_of_locals;
        for(int i = 0; i < locals; i++) {
          Object* local = scope->get_local(i);
          f(state, &local);
          scope->set_local(i, local);
        }

        f(state, reinterpret_cast<Object**>(&scope->last_match_));
        f(state, reinterpret_cast<Object**>(&scope->parent_));
        f(state, reinterpret_cast<Object**>(&scope->on_heap_));
      }

      frame = frame->previous;
    }
  }
}
