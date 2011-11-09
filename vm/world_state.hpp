
#include "util/thread.hpp"
#include "gc/managed.hpp"

namespace rubinius {
  class WorldState {
    thread::Mutex mutex_;
    thread::Condition waiting_to_stop_;
    thread::Condition waiting_to_run_;
    int pending_threads_;
    bool should_stop_;

    atomic::integer time_waiting_;

  public:
    WorldState()
      : pending_threads_(0)
      , should_stop_(false)
      , time_waiting_(0)
    {}

    uint64_t time_waiting() {
      return time_waiting_.read();
    }

    /**
     * Called after a fork(), when we know we're alone again, to get
     * everything back in the proper order.
     */
    void reinit() {
      mutex_.init();
      waiting_to_stop_.init();
      waiting_to_run_.init();
      pending_threads_ = 1;
      should_stop_ = false;
    }


    /**
     * If called when the GC is waiting to run, wait until the GC tells us its
     * OK to continue. Always decrements pending_threads_ at the end.
     */
    void become_independent(THREAD) {
      thread::Mutex::LockGuard guard(mutex_);

      switch(state->run_state()) {
      case ManagedThread::eAlone:
        // Running alone, ignore.
        return;
      case ManagedThread::eIndependent:
        // Already independent, ignore.
        return;
      case ManagedThread::eSuspended:
        // This is sort of bad. We're already suspended
        // and want to go independent. Abort on this.
        rubinius::bug("Trying to make a suspended thread independent");
        break;
      case ManagedThread::eRunning:
        // If someone is waiting on us to stop, stop now.
        if(should_stop_) wait_to_run(state);

        // We're now independent.
        state->run_state_ = ManagedThread::eIndependent;
        pending_threads_--;
        break;
      }
    }

    void become_dependent(THREAD) {
      thread::Mutex::LockGuard guard(mutex_);

      switch(state->run_state()) {
      case ManagedThread::eAlone:
        // Running alone, ignore.
        return;
      case ManagedThread::eRunning:
        // Ignore this, a running thread is already dependent.
        return;
      case ManagedThread::eSuspended:
        // Again, bad, don't allow this.
        rubinius::bug("Trying to make a suspended thread dependent");
        break;
      case ManagedThread::eIndependent:
        // If the GC is running, wait here...
        while(should_stop_) {
          waiting_to_run_.wait(mutex_);
        }

        // Ok, we're running again.
        state->run_state_ = ManagedThread::eRunning;
        pending_threads_++;
      }
    }

    void ask_for_stopage() {
      thread::Mutex::LockGuard guard(mutex_);
      should_stop_ = true;

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD requested stopage: " << pending_threads_ << "]\n";
      }
    }

    void wait_til_alone(THREAD) {
      thread::Mutex::LockGuard guard(mutex_);
      should_stop_ = true;

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD waiting until alone]\n";
      }

      if(state->run_state_ != ManagedThread::eRunning) {
        rubinius::bug("A non-running thread is trying to wait till alone");
      }

      // For ourself..
      pending_threads_--;

      timer::Running<> timer(time_waiting_);

      while(pending_threads_ > 0) {
        if(cDebugThreading) {
          std::cerr << "[" << VM::current() << " WORLD waiting on condvar: "
                    << pending_threads_ << "]\n";
        }
        waiting_to_stop_.wait(mutex_);
      }

      state->run_state_ = ManagedThread::eAlone;

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD o/~ I think we're alone now.. o/~]\n";
      }
    }

    void stop_threads_externally() {
      thread::Mutex::LockGuard guard(mutex_);
      should_stop_ = true;

      if(cDebugThreading) {
        std::cerr << "[WORLD stopping all threads (as external event)]\n";
      }

      while(pending_threads_ > 0) {
        if(cDebugThreading) {
          std::cerr << "[" << VM::current() << " WORLD waiting on condvar: "
                    << pending_threads_ << "]\n";
        }
        waiting_to_stop_.wait(mutex_);
      }

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD o/~ I think we're alone now.. o/~]\n";
      }
    }

    void wake_all_waiters(THREAD) {
      thread::Mutex::LockGuard guard(mutex_);
      should_stop_ = false;

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD waking all threads]\n";
      }

      if(state->run_state_ != ManagedThread::eAlone) {
        rubinius::bug("A non-alone thread is trying to wake all");
      }

      // For ourself..
      pending_threads_++;

      waiting_to_run_.broadcast();

      state->run_state_ = ManagedThread::eRunning;
    }

    void restart_threads_externally() {
      thread::Mutex::LockGuard guard(mutex_);
      should_stop_ = false;

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD waking all threads (externally)]\n";
      }

      waiting_to_run_.broadcast();
    }

    bool should_stop() {
      thread::Mutex::LockGuard guard(mutex_);
      return should_stop_;
    }

    bool checkpoint(THREAD) {
      // Test should_stop_ without the lock, because we do this a lot.
      if(should_stop_) {
        thread::Mutex::LockGuard guard(mutex_);
        // If the thread is set to alone, then ignore checkpointing
        if(state->run_state() == ManagedThread::eAlone) return false;
        if(should_stop_) wait_to_run(state);
        return true;
      }

      return false;
    }

  private:
    void wait_to_run(THREAD) {
      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD stopping, waiting to be restarted]\n";
      }

      if(state->run_state_ != ManagedThread::eRunning) {
        rubinius::bug("Suspending a non running thread!");
      }

      state->run_state_ = ManagedThread::eSuspended;
      pending_threads_--;
      waiting_to_stop_.signal();

      while(should_stop_) {
        waiting_to_run_.wait(mutex_);
      }

      pending_threads_++;
      state->run_state_ = ManagedThread::eRunning;

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD restarted]\n";
      }
    }
  };
}
