#ifndef RBX_SIGNAL_HPP
#define RBX_SIGNAL_HPP

#include "lock.hpp"
#include "auxiliary_threads.hpp"

#include "gc/root.hpp"

#include <list>

namespace rubinius {
  class VM;
  class State;
  struct CallFrame;
  class Thread;

  Object* handle_tramp(STATE);

  class SignalHandler : public AuxiliaryThread, public Lockable {
    SharedState& shared_;
    VM* target_;
    VM* self_;

    int pending_signals_[NSIG];
    int queued_signals_;
    utilities::thread::SpinLock lock_;
    int read_fd_;
    int write_fd_;
    bool exit_;

    TypedRoot<Thread*> thread_;

    std::list<int> watched_signals_;

  public:
    enum HandlerType {
      eDefault,
      eIgnore,
      eCustom
    };

    SignalHandler(STATE);
    virtual ~SignalHandler();

    void perform(State*);

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

    void run(State*);
  };
}

#endif
