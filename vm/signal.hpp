#ifndef RBX_SIGNAL_HPP
#define RBX_SIGNAL_HPP

#include "lock.hpp"

#include "gc/root.hpp"

#include <list>

namespace rubinius {
  class VM;
  class State;
  struct CallFrame;
  class Thread;

  class SignalHandler : public Lockable {
    VM* target_;
    VM* self_;

    int pending_signals_[NSIG];
    int queued_signals_;
    thread::SpinLock lock_;
    int read_fd_;
    int write_fd_;
    bool exit_;

    TypedRoot<Thread*> thread_;

  public:
    enum HandlerType {
      eDefault,
      eIgnore,
      eCustom
    };

    SignalHandler(STATE);

    void perform(State*);

    void add_signal(State*, int sig, HandlerType type = eCustom);
    void handle_signal(int sig);
    static void signal_tramp(int sig);

    bool deliver_signals(STATE, CallFrame* call_frame);

    void reopen_pipes();

    static void on_fork(STATE, bool full=true);
    void on_fork_i(STATE, bool full);

    static void shutdown();
    void shutdown_i();

    void run(State*);
  };
}

#endif
