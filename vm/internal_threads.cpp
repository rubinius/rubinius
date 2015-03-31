#include "vm.hpp"
#include "prelude.hpp"
#include "environment.hpp"
#include "internal_threads.hpp"

#include "builtin/native_method.hpp"

#include "dtrace/dtrace.h"
#include "util/logger.hpp"

namespace rubinius {
  using namespace utilities;

#define AUXILIARY_THREAD_STACK_SIZE   0x100000

  InternalThread::InternalThread(STATE, std::string name)
    : vm_(state->shared().new_vm())
    , name_(name)
    , thread_running_(false)
    , metrics_(vm_->metrics())
    , thread_exit_(false)
  {
    state->shared().internal_threads()->register_thread(this);
  }

  void* InternalThread::run(void* ptr) {
    InternalThread* thread = reinterpret_cast<InternalThread*>(ptr);
    VM* vm = thread->vm();

    SharedState& shared = vm->shared;
    State state_obj(vm), *state = &state_obj;

    state->vm()->set_run_state(ManagedThread::eIndependent);

    RBX_DTRACE_CHAR_P thread_name =
      const_cast<RBX_DTRACE_CHAR_P>(thread->name_.c_str());
    vm->set_name(thread_name);

    RUBINIUS_THREAD_START(const_cast<RBX_DTRACE_CHAR_P>(thread_name),
                          vm->thread_id(), 1);

    NativeMethod::init_thread(state);

    thread->thread_running_ = true;

    thread->run(state);

    thread->thread_running_ = false;

    NativeMethod::cleanup_thread(state);

    RUBINIUS_THREAD_STOP(const_cast<RBX_DTRACE_CHAR_P>(thread_name),
                         vm->thread_id(), 1);

    if(state->vm()->run_state() != ManagedThread::eIndependent) {
      shared.gc_independent();
    }

    return 0;
  }

  void InternalThread::initialize(STATE) {
    thread_exit_ = false;
    thread_running_ = false;
  }

  void InternalThread::start(STATE) {
    initialize(state);
    start_thread(state);
  }

  void InternalThread::start_thread(STATE) {
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, AUXILIARY_THREAD_STACK_SIZE);

    if(int error = pthread_create(&vm_->os_thread(), &attrs,
          InternalThread::run, (void*)this)) {
      logger::error("%s: %s: create thread failed", strerror(error), name_.c_str());
    }
  }

  void InternalThread::wakeup(STATE) {
    thread_exit_ = true;
    atomic::memory_barrier();
  }

  void InternalThread::stop_thread(STATE) {
    GCIndependent guard(state, 0);

    wakeup(state);

    void* return_value;
    pthread_t os = vm_->os_thread();
    pthread_join(os, &return_value);
  }

  void InternalThread::stop(STATE) {
    state->shared().internal_threads()->unregister_thread(this);

    stop_thread(state);
    VM::discard(state, vm_);
  }

  void InternalThread::after_fork_child(STATE) {
    vm_ = state->shared().new_vm();
    start(state);
  }

  void InternalThreads::register_thread(InternalThread* thread) {
    threads_.push_back(thread);
  }

  void InternalThreads::unregister_thread(InternalThread* thread) {
    threads_.remove(thread);
  }

  void InternalThreads::shutdown(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(shutdown_in_progress_) return;
    shutdown_in_progress_ = true;

    while(!threads_.empty()) {
      threads_.front()->stop(state);
    }

    shutdown_in_progress_ = false;
  }

  void InternalThreads::before_exec(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(exec_in_progress_) return;
    exec_in_progress_ = true;

    for(std::list<InternalThread*>::reverse_iterator i = threads_.rbegin();
        i != threads_.rend();
        ++i) {
      (*i)->before_exec(state);
    }

    state->shared().env()->before_exec(state);
  }

  void InternalThreads::after_exec(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after execvp() call.
    state->shared().env()->after_exec(state);

    for(std::list<InternalThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_exec(state);
    }

    exec_in_progress_ = false;
  }

  void InternalThreads::before_fork_exec(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(fork_exec_in_progress_) return;
    fork_exec_in_progress_ = true;

    for(std::list<InternalThread*>::reverse_iterator i = threads_.rbegin();
        i != threads_.rend();
        ++i) {
      (*i)->before_fork_exec(state);
    }
  }

  void InternalThreads::after_fork_exec_parent(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after fork() call.
    for(std::list<InternalThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_exec_parent(state);
    }

    fork_in_progress_ = false;
  }

  void InternalThreads::after_fork_exec_child(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after execvp() call.
    for(std::list<InternalThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_exec_child(state);
    }

    fork_exec_in_progress_ = false;
  }

  void InternalThreads::before_fork(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(fork_in_progress_) return;
    fork_in_progress_ = true;

    for(std::list<InternalThread*>::reverse_iterator i = threads_.rbegin();
        i != threads_.rend();
        ++i) {
      (*i)->before_fork(state);
    }
  }

  void InternalThreads::after_fork_parent(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after fork() call.
    for(std::list<InternalThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_parent(state);
    }

    fork_in_progress_ = false;
  }

  void InternalThreads::after_fork_child(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after fork() call.
    for(std::list<InternalThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_child(state);
    }

    fork_in_progress_ = false;
  }

  void InternalThreads::init() {
    mutex_.init();
  }
}
