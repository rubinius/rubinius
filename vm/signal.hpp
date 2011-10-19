#ifndef RBX_SIGNAL_HPP
#define RBX_SIGNAL_HPP

#include "lock.hpp"

#include "gc/root.hpp"

#include <list>

namespace rubinius {
  class VM;
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

    SignalHandler(VM* vm);

    void perform(VM*);

    void add_signal(VM*, int sig, HandlerType type = eCustom);
    void handle_signal(int sig);
    static void signal_tramp(int sig);

    bool deliver_signals(VM*, CallFrame* call_frame);

    void reopen_pipes();

    static void on_fork(VM*, bool full=true);
    void on_fork_i(VM*, bool full);

    static void shutdown();
    void shutdown_i();

    void run(VM*);
  };
}

#endif
