#include "vm.hpp"
#include "prelude.hpp"
#include "environment.hpp"

#include "auxiliary_threads.hpp"

#include "dtrace/dtrace.h"
#include "util/logger.hpp"

namespace rubinius {
  using namespace utilities;

#define AUXILIARY_THREAD_STACK_SIZE   0x100000

  AuxiliaryThread::AuxiliaryThread(STATE, std::string name)
    : vm_(state->shared().new_vm())
    , name_(name)
    , thread_running_(false)
    , metrics_(vm_->metrics())
    , thread_exit_(false)
  {
    state->shared().auxiliary_threads()->register_thread(this);
  }

  void* AuxiliaryThread::run(void* ptr) {
    AuxiliaryThread* thread = reinterpret_cast<AuxiliaryThread*>(ptr);
    VM* vm = thread->vm();

    SharedState& shared = vm->shared;
    State state_obj(vm), *state = &state_obj;

    RBX_DTRACE_CHAR_P thread_name =
      const_cast<RBX_DTRACE_CHAR_P>(thread->name_.c_str());
    vm->set_name(thread_name);

    RUBINIUS_THREAD_START(const_cast<RBX_DTRACE_CHAR_P>(thread_name),
                          vm->thread_id(), 1);

    thread->thread_running_ = true;

    thread->run(state);

    thread->thread_running_ = false;

    RUBINIUS_THREAD_STOP(const_cast<RBX_DTRACE_CHAR_P>(thread_name),
                         vm->thread_id(), 1);

    shared.gc_independent();

    return 0;
  }

  void AuxiliaryThread::initialize(STATE) {
    thread_exit_ = false;
    thread_running_ = false;
  }

  void AuxiliaryThread::start(STATE) {
    initialize(state);
    start_thread(state);
  }

  void AuxiliaryThread::start_thread(STATE) {
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, AUXILIARY_THREAD_STACK_SIZE);

    if(int error = pthread_create(&vm_->os_thread(), &attrs,
          AuxiliaryThread::run, (void*)this)) {
      logger::error("%s: %s: create thread failed", strerror(error), name_.c_str());
    }
  }

  void AuxiliaryThread::wakeup(STATE) {
    thread_exit_ = true;
    atomic::memory_barrier();
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

  void AuxiliaryThread::stop(STATE) {
    stop_thread(state);
  }

  void AuxiliaryThread::after_fork_child(STATE) {
    start(state);
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
      (*i)->stop(state);
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
