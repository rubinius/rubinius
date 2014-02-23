#ifndef RBX_SIGNAL_HPP
#define RBX_SIGNAL_HPP

#include "lock.hpp"
#include "auxiliary_threads.hpp"

#include "gc/root.hpp"

#include <list>
#include <string>

namespace rubinius {
  class VM;
  class State;
  class Configuration;
  struct CallFrame;
  class Thread;

  class SignalHandler : public AuxiliaryThread, public Lockable {
    SharedState& shared_;
    VM* target_;
    VM* self_;

    int pending_signals_[NSIG];
    int queued_signals_;

    bool paused_;
    bool exit_;

    TypedRoot<Thread*> thread_;

    std::list<int> watched_signals_;

    utilities::thread::Condition worker_cond_;
    utilities::thread::Condition pause_cond_;
    utilities::thread::Mutex worker_lock_;

  public:
    enum HandlerType {
      eDefault,
      eIgnore,
      eCustom
    };

    SignalHandler(STATE, Configuration& config);
    virtual ~SignalHandler();

    void setup_default_handlers(std::string path);

    void perform(STATE);

    void add_signal(State*, int sig, HandlerType type = eCustom);
    void handle_signal(int sig);
    static void signal_tramp(int sig);

    bool deliver_signals(STATE, CallFrame* call_frame);

    void open_pipes();
    void start_thread(STATE);
    void stop_thread(STATE);

    void shutdown(STATE);
    void before_exec(STATE);
    void after_exec(STATE);
    void before_fork(STATE);
    void after_fork_parent(STATE);
    void after_fork_child(STATE);

    void run(STATE);
  };
}

#endif
