#include "vm.hpp"
#include "state.hpp"
#include "defines.hpp"
#include "environment.hpp"
#include "machine_threads.hpp"
#include "thread_phase.hpp"

#include "class/native_method.hpp"

#include "dtrace/dtrace.h"
#include "logger.hpp"

namespace rubinius {
  using namespace utilities;

  MachineThread::MachineThread(STATE, std::string name, StackSize stack_size)
    : vm_(state->shared().thread_nexus()->new_vm(&state->shared(), name.c_str()))
    , stack_size_(stack_size)
    , thread_running_(false)
    , thread_exit_(false)
  {
    vm_->set_kind(memory::ManagedThread::eSystem);
    state->shared().machine_threads()->register_thread(this);
  }

  void* MachineThread::run(void* ptr) {
    MachineThread* thread = reinterpret_cast<MachineThread*>(ptr);

    VM* vm = thread->vm();
    State state_obj(vm), *state = &state_obj;

    vm->set_current_thread();

    RUBINIUS_THREAD_START(
        const_cast<RBX_DTRACE_CHAR_P>(vm->name().c_str()), vm->thread_id(), 1);

    vm->set_stack_bounds(thread->stack_size_);

    NativeMethod::init_thread(state);

    thread->thread_running_ = true;

    thread->run(state);

    thread->thread_running_ = false;

    NativeMethod::cleanup_thread(state);

    RUBINIUS_THREAD_STOP(
        const_cast<RBX_DTRACE_CHAR_P>(vm->name().c_str()), vm->thread_id(), 1);

    vm->set_call_frame(NULL);
    vm->unmanaged_phase(state);

    vm->set_zombie(state);

    return 0;
  }

  void MachineThread::initialize(STATE) {
    thread_exit_ = false;
    thread_running_ = false;
  }

  void MachineThread::start(STATE) {
    initialize(state);
    start_thread(state);
  }

  void MachineThread::start_thread(STATE) {
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, stack_size_);

    if(int error = pthread_create(&vm_->os_thread(), &attrs,
          MachineThread::run, (void*)this)) {
      logger::fatal("%s: %s: create thread failed", strerror(error), vm_->name().c_str());
      ::abort();
    }

    pthread_attr_destroy(&attrs);
  }

  void MachineThread::wakeup(STATE) {
    thread_exit_ = true;
    atomic::memory_barrier();
  }

  void MachineThread::stop_thread(STATE) {
    UnmanagedPhase unmanaged(state);

    wakeup(state);

    void* return_value;
    pthread_t os = vm_->os_thread();
    pthread_join(os, &return_value);
  }

  void MachineThread::stop(STATE) {
    state->shared().machine_threads()->unregister_thread(this);

    stop_thread(state);
    VM::discard(state, vm_);
  }

  void MachineThread::after_fork_child(STATE) {
    vm_ = state->shared().thread_nexus()->new_vm(&state->shared());
    start(state);
  }

  void MachineThreads::register_thread(MachineThread* thread) {
    threads_.push_back(thread);
  }

  void MachineThreads::unregister_thread(MachineThread* thread) {
    threads_.remove(thread);
  }

  void MachineThreads::shutdown(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(shutdown_in_progress_) return;
    shutdown_in_progress_ = true;

    while(!threads_.empty()) {
      threads_.front()->stop(state);
    }

    shutdown_in_progress_ = false;
  }

  void MachineThreads::before_exec(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(exec_in_progress_) return;
    exec_in_progress_ = true;

    for(MachineThreadList::reverse_iterator i = threads_.rbegin();
        i != threads_.rend();
        ++i) {
      (*i)->before_exec(state);
    }
  }

  void MachineThreads::after_exec(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after execvp() call.
    state->shared().env()->after_exec(state);

    for(MachineThreadList::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_exec(state);
    }

    exec_in_progress_ = false;
  }

  void MachineThreads::before_fork_exec(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(fork_exec_in_progress_) return;
    fork_exec_in_progress_ = true;

    for(MachineThreadList::reverse_iterator i = threads_.rbegin();
        i != threads_.rend();
        ++i) {
      (*i)->before_fork_exec(state);
    }
  }

  void MachineThreads::after_fork_exec_parent(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after fork() call.
    for(MachineThreadList::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_exec_parent(state);
    }

    fork_in_progress_ = false;
  }

  void MachineThreads::after_fork_exec_child(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after execvp() call.
    for(MachineThreadList::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_exec_child(state);
    }

    fork_exec_in_progress_ = false;
  }

  void MachineThreads::before_fork(STATE) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    if(fork_in_progress_) return;
    fork_in_progress_ = true;

    for(MachineThreadList::reverse_iterator i = threads_.rbegin();
        i != threads_.rend();
        ++i) {
      (*i)->before_fork(state);
    }
  }

  void MachineThreads::after_fork_parent(STATE) {
    // We don't guard here on the assumption that only one thread is running
    // after fork() call.
    for(MachineThreadList::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_parent(state);
    }

    fork_in_progress_ = false;
  }

  void MachineThreads::after_fork_child(STATE) {
    mutex_.init();

    // We don't guard here on the assumption that only one thread is running
    // after fork() call.
    for(MachineThreadList::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      (*i)->after_fork_child(state);
    }

    fork_in_progress_ = false;
  }
}
