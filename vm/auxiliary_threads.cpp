#include "prelude.hpp"
#include "auxiliary_threads.hpp"

namespace rubinius {
  void AuxiliaryThreads::register_thread(AuxiliaryThread* thread) {
    threads_.push_back(thread);
  }

  void AuxiliaryThreads::unregister_thread(AuxiliaryThread* thread) {
    threads_.remove(thread);
  }

  void AuxiliaryThreads::shutdown(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(shutdown_in_progress_) return;
    shutdown_in_progress_ = true;

    for(std::list<AuxiliaryThread*>::reverse_iterator i = threads_.rbegin();
        i != threads_.rend();
        ++i) {
      (*i)->shutdown(state);
    }

    shutdown_in_progress_ = false;
  }

  void AuxiliaryThreads::before_exec(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(exec_in_progress_) return;
    exec_in_progress_ = true;

    for(std::list<AuxiliaryThread*>::reverse_iterator i = threads_.rbegin();
        i != threads_.rend();
        ++i) {
      (*i)->before_exec(state);
    }
  }

  void AuxiliaryThreads::after_exec(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after execvp() call.
    for(std::list<AuxiliaryThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_exec(state);
    }

    exec_in_progress_ = false;
  }

  void AuxiliaryThreads::before_fork(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(fork_in_progress_) return;
    fork_in_progress_ = true;

    for(std::list<AuxiliaryThread*>::reverse_iterator i = threads_.rbegin();
        i != threads_.rend();
        ++i) {
      (*i)->before_fork(state);
    }
  }

  void AuxiliaryThreads::after_fork_parent(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after fork() call.
    for(std::list<AuxiliaryThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_parent(state);
    }

    fork_in_progress_ = false;
  }

  void AuxiliaryThreads::after_fork_child(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after fork() call.
    for(std::list<AuxiliaryThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_child(state);
    }

    fork_in_progress_ = false;
  }

  void AuxiliaryThreads::init() {
    mutex_.init();
  }
}
