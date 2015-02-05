#include "vm.hpp"
#include "prelude.hpp"
#include "environment.hpp"

#include "auxiliary_threads.hpp"

namespace rubinius {
  void AuxiliaryThread::shutdown(STATE) {
    stop_thread(state);
  }

  void AuxiliaryThread::stop_thread(STATE) {
    if(vm_) {
      wakeup(state);

      if(atomic::poll(thread_running_, false)) {
        void* return_value;
        pthread_t os = vm_->os_thread();
        pthread_join(os, &return_value);
        VM::discard(state, vm_);
      }

      vm_ = NULL;
    }
  }


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

    state->shared().env()->before_exec(state);
  }

  void AuxiliaryThreads::after_exec(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after execvp() call.
    state->shared().env()->after_exec(state);

    for(std::list<AuxiliaryThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_exec(state);
    }

    exec_in_progress_ = false;
  }

  void AuxiliaryThreads::before_fork_exec(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(fork_exec_in_progress_) return;
    fork_exec_in_progress_ = true;

    for(std::list<AuxiliaryThread*>::reverse_iterator i = threads_.rbegin();
        i != threads_.rend();
        ++i) {
      (*i)->before_fork_exec(state);
    }
  }

  void AuxiliaryThreads::after_fork_exec_parent(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after fork() call.
    for(std::list<AuxiliaryThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_exec_parent(state);
    }

    fork_in_progress_ = false;
  }

  void AuxiliaryThreads::after_fork_exec_child(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after execvp() call.
    for(std::list<AuxiliaryThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_exec_child(state);
    }

    fork_exec_in_progress_ = false;
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
