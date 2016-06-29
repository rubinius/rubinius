#ifndef RBX_SIGNAL_HPP
#define RBX_SIGNAL_HPP

#include "logger.hpp"
#include "memory/root.hpp"

#include "util/thread.hpp"

#include <list>
#include <string>

namespace rubinius {
  class Object;
  class VM;
  class State;
  class Configuration;
  struct CallFrame;

  class SignalThread {
    SharedState& shared_;
    VM* vm_;

    bool system_exit_;
    int exit_code_;

    const static int pending_signal_size_ = 256;

    int pending_signals_[pending_signal_size_];
    int queue_index_;
    int process_index_;

    std::list<int> watched_signals_;
    utilities::thread::SpinLock watch_lock_;

    utilities::thread::Condition condition_;
    utilities::thread::Mutex lock_;

  public:
    const static int stack_size = 0x100000;

    enum HandlerType {
      eDefault,
      eIgnore,
      eCustom
    };

    SignalThread(STATE, VM* vm);

    SharedState& shared() {
      return shared_;
    }

    VM* vm() {
      return vm_;
    }

    void system_exit(Object* exit_code) {
      utilities::thread::Mutex::LockGuard guard(lock_);

      set_exit_code(exit_code);
      system_exit_ = true;
      atomic::memory_barrier();

      condition_.signal();
    }

    void set_exit_code(Object* exit_code);

    static VM* new_vm(STATE);
    static void* run(void* ptr);
    static void signal_handler(int signal);

    void install_default_handlers();

    void initialize(STATE);
    void start(STATE);
    void restart(STATE);
    void run(STATE);
    void stop(STATE);
    void after_fork_child(STATE);

    void queue_signal(int signal);
    void add_signal_handler(State*, int signal, HandlerType type = eCustom);

    void print_backtraces();

    void print_machine_info(logger::PrintFunction function);
    void print_process_info(logger::PrintFunction function);
  };
}

#endif
