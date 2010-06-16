#ifndef RBX_SIGNAL_HPP
#define RBX_SIGNAL_HPP

#include "util/thread.hpp"

#include <list>

namespace rubinius {
  class VM;
  struct CallFrame;

  class SignalHandler : public thread::Thread {
    VM* vm_;
    int pending_signals_[NSIG];
    int running_signals_[NSIG];
    int queued_signals_;
    bool executing_signal_;
    thread::SpinLock lock_;
    int read_fd_;
    int write_fd_;
    bool exit_;

  public:
    SignalHandler(VM* vm);

    void perform();

    void add_signal(int sig, bool def=false);
    void handle_signal(int sig);
    static void signal_tramp(int sig);

    void deliver_signals(CallFrame* call_frame);

    void reopen_pipes();
    static void on_fork();
    static void shutdown();
    void shutdown_i();
  };
}

#endif
