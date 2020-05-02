#ifndef RBX_MACHINE_H
#define RBX_MACHINE_H

#include "defines.hpp"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <list>
#include <mutex>

namespace rubinius {
  class Fixnum;
  class Object;
  class Machine;
  class Thread;

  class ngLogger { };
  class Environment;
  class Configuration;

  class Diagnostics;

  namespace diagnostics {
    class Diagnostic;
  }

  class Memory;

  namespace memory {
    class Collector;
  }

  class Signals;
  class ngCodeDB { };

  class C_API;

  namespace jit {
    class MachineCompiler;
  }

  class ngDebugger { };

  class Profiler {
  public:
    Profiler() { }
    virtual ~Profiler() { }
  };

  class Console;

  class MachineState {
  public:
    enum Phase {
      eBooting,
      eRunning,
      eHalting,
    };

  private:
    ThreadState* _main_thread_;
    Object* _loader_;
    uint64_t _start_time_;
    uint32_t _hash_seed_;
    int _exit_code_;
    std::atomic<Phase> _phase_;

  public:
    MachineState();
    virtual ~MachineState() { }

    const ThreadState* main_thread() {
      return _main_thread_;
    }

    Object* loader() {
      return _loader_;
    }

    void set_loader(Object* loader) {
      _loader_ = loader;
    }

    const uint32_t hash_seed() {
      return _hash_seed_;
    }

    const int exit_code() {
      return _exit_code_;
    }

    void exit_code(int code) {
      _exit_code_ = code;
    }

    bool booting_p() {
      return _phase_ == eBooting;
    }

    bool running_p() {
      return _phase_ == eRunning;
    }

    bool halting_p() {
      return _phase_ == eHalting;
    }

    void set_halting() {
      _phase_ = eHalting;
    }

    void set_start_time();
    double run_time();
  };

  class Threads {
  public:
    typedef std::list<ThreadState*> ThreadList;

  private:
    Machine* machine_;
    ThreadList threads_;
    std::mutex threads_mutex_;
    uint32_t thread_ids_;

  public:
    Threads(Machine* machine)
      : machine_(machine)
      , threads_()
      , threads_mutex_()
      , thread_ids_(0)
    { }

    virtual ~Threads() { }

    ThreadState* create_thread_state(const char* name = NULL);
    void remove_thread_state(ThreadState* thread_state);

    void after_fork_child(STATE);

    void each(STATE, std::function<void (STATE, ThreadState*)> f);
  };

  class Machine {
    MachineState* _machine_state_;
    ngLogger* _logger_;
    Threads* _threads_;
    Configuration* _configuration_;
    Environment* _environment_;
    Diagnostics* _diagnostics_;
    Memory* _memory_;
    memory::Collector* _collector_;
    Signals* _signals_;
    ngCodeDB* _codedb_;
    C_API* _c_api_;
    jit::MachineCompiler* _compiler_;
    ngDebugger* _debugger_;
    Profiler* _profiler_;
    Console* _console_;

    static std::mutex _waiting_mutex_;
    static std::condition_variable _waiting_condition_;
    static std::atomic<uint32_t> _threads_lock_;
    static std::atomic<uint64_t> _halting_;
    static std::atomic<bool> _stop_;

  public:
    Machine(int argc, char** argv);
    virtual ~Machine();

    std::mutex& waiting_mutex() {
      return _waiting_mutex_;
    }

    std::condition_variable& waiting_condition() {
      return _waiting_condition_;
    }

    std::atomic<uint32_t>& threads_lock() {
      return _threads_lock_;
    }

    std::atomic<uint64_t>& halting() {
      return _halting_;
    }

    std::atomic<bool>& stop() {
      return _stop_;
    }

    MachineState* const machine_state() {
      return _machine_state_;
    }

    Threads* const threads() {
      return _threads_;
    }

    Configuration* const configuration() {
      return _configuration_;
    }

    Environment* const environment() const {
      return _environment_;
    }

    Diagnostics* const diagnostics() {
      return _diagnostics_;
    }

    Memory* const memory() {
      return _memory_;
    }

    memory::Collector* const collector() {
      return _collector_;
    }

    Signals* const signals() {
      return _signals_;
    }

    C_API* const c_api() {
      return _c_api_;
    }

    jit::MachineCompiler* const compiler() {
      return _compiler_;
    }

    Profiler* const profiler() {
      return _profiler_;
    }

    Console* const console() {
      return _console_;
    }

    void boot();

    int halt(STATE, Object* exit_code);
    int halt(int exit_code=0);
    int halt(STATE, int exit_code=0);

    void before_fork(STATE);
    void after_fork_parent(STATE);
    void after_fork_child(STATE);

    void trace_objects(STATE, std::function<void (STATE, Object**)> f);

    Diagnostics* start_diagnostics(STATE);
    void report_diagnostics(diagnostics::Diagnostic* diagnostic);

    jit::MachineCompiler* start_compiler(STATE);

    void halt_console(STATE);
    void halt_profiler(STATE);
    void halt_debugger(STATE);
    void halt_compiler(STATE);
    void halt_c_api(STATE);
    void halt_codedb(STATE);
    void halt_signals(STATE);
    void halt_collector(STATE);
    void halt_memory(STATE);
    void halt_threads(STATE);
    void halt_diagnostics(STATE);
    void halt_configuration(STATE);
    void halt_environment(STATE);
    void halt_logger(STATE);
    void halt_machine_state(STATE);
    void halt_machine(STATE);
  };
}
#endif
