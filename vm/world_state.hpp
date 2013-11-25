
#include "util/thread.hpp"
#include "gc/managed.hpp"

namespace rubinius {
  class WorldState {
    utilities::thread::Mutex mutex_;
    utilities::thread::Condition waiting_to_run_;
    int pending_threads_;
    int should_stop_;
    bool* check_global_interrupts_;

    atomic::integer time_waiting_;

  public:
    WorldState(bool* check_global_interrupts)
      : pending_threads_(0)
      , should_stop_(0)
      , check_global_interrupts_(check_global_interrupts)
      , time_waiting_(0)
    {
      mutex_.init();
      waiting_to_run_.init();
    }

    uint64_t time_waiting() {
      return time_waiting_.read();
    }

    /**
     * Called after a fork(), when we know we're alone again, to get
     * everything back in the proper order.
     */
    void reinit() {
      // When we're reinitting the world state, we're stopped
      // so we have to initialize pending_threads_ to 0 and
      // should_stop to 1 so we start off in the proper state
      // and can continue after a fork.
      pending_threads_ = 0;
      should_stop_ = 1;
      mutex_.init();
      waiting_to_run_.init();
    }


    /**
     * If called when the GC is waiting to run, wait until the GC tells us its
     * OK to continue. Always decrements pending_threads_ at the end.
     */
    void become_independent(THREAD) {
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
        // We're now independent.
        state->run_state_ = ManagedThread::eIndependent;
        atomic::fetch_and_sub(&pending_threads_, 1);
        break;
      }
    }

    /**
     * Called when a thread is shutting down. This is done so it needs
     * no access to a VM structure since that is already deallocated while
     * still being gc dependent.
     */
    void become_independent() {
      atomic::fetch_and_sub(&pending_threads_, 1);
    }

    void become_dependent(THREAD, utilities::thread::Condition* cond = NULL) {
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
        for(;;) {
          // If the GC is running, wait here...
          if(should_stop_) {
            utilities::thread::Mutex::LockGuard guard(mutex_);
            state->run_state_ = ManagedThread::eSuspended;
            if(cond) cond->broadcast();
            // We need to grab the mutex because we might want
            // to wait here.
            while(should_stop_) {
              waiting_to_run_.wait(mutex_);
            }
          }

          // Ok, we're running again.
          state->run_state_ = ManagedThread::eRunning;
          atomic::fetch_and_add(&pending_threads_, 1);
          // After decreasing the thread count, we have to check whether
          // we might have to stop. This is basically because
          // there is a race condition here that should_stop isn't true
          // yet the first time before incrementing. However, another thread
          // waiting to GC could have set should_stop and not seen the
          // increment of this thread yet, thinking it's safe to GC.
          //
          if(!atomic::read(&should_stop_)) return;
          // If we do have to stop, subtract one from the thread count
          // and retry again. This will make the thread go into the wait.
          atomic::fetch_and_sub(&pending_threads_, 1);
        }
      }
    }

    bool wait_til_alone(THREAD) {
      if(!atomic::compare_and_swap(&should_stop_, 0, 1)) {
        if(cDebugThreading) {
          std::cerr << "[" << VM::current()
                    << " WORLD detected concurrent stop request, returning false]\n";
        }
        return false;
      }

      *check_global_interrupts_ = true;
      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD waiting until alone]\n";
      }

      if(state->run_state_ != ManagedThread::eRunning) {
        rubinius::bug("A non-running thread is trying to wait till alone");
      }

      state->run_state_ = ManagedThread::eAlone;
      // For ourself..
      atomic::fetch_and_sub(&pending_threads_, 1);

      timer::Running<> timer(time_waiting_);

      // We need a write barrier so we're sure we're seeing an up to
      // date version of pending_threads_ in each loop.
      while(atomic::read(&pending_threads_) > 0) {
        if(cDebugThreading) {
          std::cerr << "[" << VM::current() << " WORLD waiting on condvar: "
                    << pending_threads_ << "]\n";
        }
        // We yield here so other threads are scheduled and can be run.
        atomic::pause();
      }

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD o/~ I think we're alone now.. o/~]\n";
      }

      return true;
    }

    void stop_threads_externally() {
      while(!atomic::compare_and_swap(&should_stop_, 0, 1)) {
        if(cDebugThreading) {
          std::cerr << "[WORLD waiting to stopping all threads (as external event)]\n";
        }
        // Wait around on the run condition variable until whoever is currently
        // working independently is done and sets should_stop_ to false.
        utilities::thread::Mutex::LockGuard guard(mutex_);
        while(should_stop_) {
          waiting_to_run_.wait(mutex_);
        }
        // We will now redo the loop to check if we can stop properly this time
      }

      if(cDebugThreading) {
        std::cerr << "[WORLD stopping all threads (as external event)]\n";
      }

      *check_global_interrupts_ = true;
      // We need a write barrier so we're sure we're seeing an up to
      // date version of pending_threads_ in each loop.
      while(atomic::read(&pending_threads_) > 0) {
        if(cDebugThreading) {
          std::cerr << "[" << VM::current() << " WORLD waiting on condvar: "
                    << pending_threads_ << "]\n";
        }
        // We yield here so other threads are scheduled and can be run.
        // We've benchmarked this and this turned out to cause the least
        // cpu burn compared to not doing anything at all here or sleeping
        // for 1 nanosecond with {0, 1}.
        atomic::pause();
      }

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD o/~ I think we're alone now.. o/~]\n";
      }
    }

    void wake_all_waiters(THREAD) {
      utilities::thread::Mutex::LockGuard guard(mutex_);

      if(!atomic::compare_and_swap(&should_stop_, 1, 0)) {
        // Ok, someone else has already restarted so we don't
        // have anything to do here anymore
        return;
      }

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD waking all threads]\n";
      }

      if(state->run_state_ != ManagedThread::eAlone) {
        rubinius::bug("A non-alone thread is trying to wake all");
      }

      *check_global_interrupts_ = false;
      // For ourself..
      atomic::fetch_and_add(&pending_threads_, 1);

      waiting_to_run_.broadcast();

      state->run_state_ = ManagedThread::eRunning;
    }

    void restart_threads_externally() {
      utilities::thread::Mutex::LockGuard guard(mutex_);
      if(!atomic::compare_and_swap(&should_stop_, 1, 0)) {
        // Ok, someone else has already restarted so we don't
        // have anything to do here anymore
        return;
      }

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD waking all threads (externally)]\n";
      }

      *check_global_interrupts_ = false;
      waiting_to_run_.broadcast();
    }

    bool should_stop() const {
      return should_stop_;
    }

    bool checkpoint(THREAD) {
      // Test should_stop_ without the lock, because we do this a lot.
      if(should_stop_) {
        // If the thread is set to alone, then ignore checkpointing
        if(state->run_state() == ManagedThread::eAlone) return false;
        wait_to_run(state);
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
      atomic::fetch_and_sub(&pending_threads_, 1);

      utilities::thread::Mutex::LockGuard guard(mutex_);
      // Ok, since we have just locked that implies a barrier
      // so we don't have to add an explicit barrier here.
      while(should_stop_) {
        waiting_to_run_.wait(mutex_);
      }

      atomic::fetch_and_add(&pending_threads_, 1);
      state->run_state_ = ManagedThread::eRunning;

      if(cDebugThreading) {
        std::cerr << "[" << VM::current() << " WORLD restarted]\n";
      }
    }
  };
}
