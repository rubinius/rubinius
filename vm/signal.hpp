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

  class SignalHandler : public InternalThread, public Lockable {
    SharedState& shared_;
    VM* target_;

    int pending_signals_[NSIG];
    int queued_signals_;

    std::list<int> watched_signals_;

    utilities::thread::Condition worker_cond_;
    utilities::thread::Mutex worker_lock_;

  public:
    enum HandlerType {
      eDefault,
      eIgnore,
      eCustom
    };

    SignalHandler(STATE, Configuration& config);

    void initialize(STATE);
    void setup_default_handlers();

    void add_signal(State*, int sig, HandlerType type = eCustom);
    void handle_signal(int sig);
    static void signal_handler(int sig);

    bool deliver_signals(STATE, CallFrame* call_frame);

    void print_backtraces();

    void open_pipes();

    void run(STATE);
    void stop(STATE);
    void wakeup(STATE);
  };
}

#endif
