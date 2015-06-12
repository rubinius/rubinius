#ifndef RBX_SIGNAL_HPP
#define RBX_SIGNAL_HPP

#include "lock.hpp"
#include "internal_threads.hpp"

#include "gc/root.hpp"

#include <list>
#include <string>

namespace rubinius {
  class VM;
  class State;
  class Configuration;
  struct CallFrame;

  class SignalThread : public InternalThread, public Lockable {
    const static int pending_signal_size_ = 256;
    SharedState& shared_;

    int pending_signals_[pending_signal_size_];
    int queue_index_;
    int process_index_;

    std::list<int> watched_signals_;
    utilities::thread::SpinLock watch_lock_;

    utilities::thread::Condition condition_;
    utilities::thread::Mutex lock_;

  public:
    enum HandlerType {
      eDefault,
      eIgnore,
      eCustom
    };

    SignalThread(STATE);

    SharedState& shared() {
      return shared_;
    }

    static void signal_handler(int signal);

    void install_default_handlers();

    void initialize(STATE);
    void run(STATE);
    void wakeup(STATE);
    void stop(STATE);

    void queue_signal(int signal);
    void add_signal_handler(State*, int signal, HandlerType type = eCustom);

    void print_backtraces();

    typedef void (*PrintFunction)(const char* message, ...);

    void print_machine_info(PrintFunction function);
    void print_process_info(PrintFunction function);
  };
}

#endif
