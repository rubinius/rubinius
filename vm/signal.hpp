#ifndef RBX_SIGNAL_HPP
#define RBX_SIGNAL_HPP

#include "util/thread.hpp"
#include "lock.hpp"

#include <list>

namespace rubinius {
  class VM;
  struct CallFrame;

  class SignalHandler : public thread::Thread, Lockable {
    VM* vm_;
    int pending_signals_[NSIG];
    int queued_signals_;
    thread::SpinLock lock_;
    int read_fd_;
    int write_fd_;
    bool exit_;

  public:
    enum HandlerType {
      eDefault,
      eIgnore,
      eCustom
    };

    SignalHandler(VM* vm);

    void perform();

    void add_signal(VM*, int sig, HandlerType type = eCustom);
    void handle_signal(int sig);
    static void signal_tramp(int sig);

    bool deliver_signals(CallFrame* call_frame);

    void reopen_pipes();
    static void on_fork();
    static void shutdown();
    void shutdown_i();
  };
}

#endif
