#ifndef RBX_SIGNAL_HPP
#define RBX_SIGNAL_HPP

#include "util/thread.hpp"

#include <list>

namespace rubinius {
  class VM;

  class SignalThread : public thread::Thread {
  public:
    SignalThread(VM*);
    ~SignalThread();

    void perform();

    // Cause the processing loop to attempt to process data
    void wake_loop();

    // The function installed as the signal handler
    static void handle_signal(int sig);

    // Logic for starting the processing of the signal
    void inform_processing(int sig);

    // Add +sig+ to the set of signals to watch for
    void add_signal(int sig);

  public: // static data
    // Used by the signal handler to inform the processing
    // loop there is data ready.
    static SignalThread* thread;

  private:
    VM* vm_;
    std::list<int> pending_signals_;

    int read_fd_;
    int write_fd_;
    sigset_t signal_set_;
    thread::SpinLock lock_;
  };
}

#endif
