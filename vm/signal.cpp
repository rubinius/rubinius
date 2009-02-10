#include "signal.hpp"
#include "vm.hpp"

#include "native_thread.hpp"

#include <iostream>
#include <sys/select.h>

namespace rubinius {
  SignalThread* SignalThread::thread = 0;

  SignalThread::SignalThread(VM* vm)
    : vm_(vm)
  {
    // Register this object as the thread handling signals.
    assert(!thread);
    thread = this;

    int fds[2];
    assert(pipe(fds) == 0);

    read_fd_  = fds[0];
    write_fd_ = fds[1];

    sigfillset(&signal_set_);
  }

  SignalThread::~SignalThread() {
    // Deregister ourself.
    thread = 0;
    close(read_fd_);
    close(write_fd_);
  }

  void SignalThread::wake_loop() {
    write(write_fd_, "!", 1);
  }

  void SignalThread::handle_signal(int sig) {
    thread->inform_processing(sig);
  }

  void SignalThread::inform_processing(int sig) {
    pending_signals_.push_back(sig);
    wake_loop();
  }

  void SignalThread::add_signal(int sig) {
    lock_.lock();
    sigdelset(&signal_set_, sig);

    // NOTE this exploits the fact that all threads share the same
    // set of signal handlers. We can call add_signal from another
    // thread, which installs the signal handler, then wakes the
    // signal thread to reinstall it's signal mask.
    //
    // When the signal arrives, the handler will be run on the signal
    // thread, since everyone else has signals blocked.
    ::signal(sig, handle_signal);
    lock_.unlock();

    wake_loop();
  }

  void SignalThread::perform() {
    lock_.lock();

    fd_set fds;
    for(;;) {
      pthread_sigmask(SIG_SETMASK, &signal_set_, NULL);

      FD_ZERO(&fds);
      FD_SET(read_fd_, &fds);

      lock_.unlock();
      ::select(read_fd_ + 1, &fds, NULL, NULL, NULL);
      lock_.lock();
      char dummy;
      read(read_fd_, &dummy, 1);
      // Hey! there must be data!
      //
      // Inhibit signals while we handle the current set.
      NativeThread::block_all_signals();

      // Read data out and forward it to the VM
      for(std::list<int>::iterator i = pending_signals_.begin();
          i != pending_signals_.end();
          i++) {
        vm_->send_async_signal(*i);
      }
    }
  }
}
