#include "thread_state.hpp"
#include "defines.hpp"
#include "environment.hpp"
#include "machine_thread.hpp"
#include "thread_phase.hpp"

#include "class/native_method.hpp"

#include "dtrace/dtrace.h"
#include "logger.hpp"

namespace rubinius {
  MachineThread::MachineThread(STATE, std::string name, StackSize stack_size)
    : thread_state_(state->thread_nexus()->create_thread_state(state->machine(), name.c_str()))
    , stack_size_(stack_size)
    , thread_running_(false)
    , thread_exit_(false)
  {
    thread_state_->set_kind(ThreadState::eSystem);
  }

  void* MachineThread::run(void* ptr) {
    MachineThread* thread = reinterpret_cast<MachineThread*>(ptr);

    ThreadState* state = thread->thread_state();

    state->set_current_thread();

    RUBINIUS_THREAD_START(
        const_cast<RBX_DTRACE_CHAR_P>(state->name().c_str()), state->thread_id(), 1);

    state->set_stack_bounds(thread->stack_size_);

    NativeMethod::init_thread(state);

    thread->thread_running_ = true;

    thread->run(state);

    thread->thread_running_ = false;

    NativeMethod::cleanup_thread(state);

    RUBINIUS_THREAD_STOP(
        const_cast<RBX_DTRACE_CHAR_P>(state->name().c_str()), state->thread_id(), 1);

    state->set_call_frame(nullptr);
    state->unmanaged_phase(state);

    // There's no way to retain a reference to this, so discard immediately.
    state->machine()->thread_nexus()->remove_thread_state(state);
    state->set_thread_dead();
    state->discard();

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

    if(int error = pthread_create(&thread_state_->os_thread(), &attrs,
          MachineThread::run, (void*)this)) {
      logger::fatal("%s: %s: create thread failed",
          strerror(error), thread_state_->name().c_str());
      ::abort();
    }

    pthread_attr_destroy(&attrs);
  }

  void MachineThread::wakeup(STATE) {
    thread_exit_ = true;
  }

  void MachineThread::stop_thread(STATE) {
    UnmanagedPhase unmanaged(state);

    wakeup(state);

    void* return_value;
    pthread_t os = thread_state_->os_thread();
    pthread_join(os, &return_value);
  }

  void MachineThread::stop(STATE) {
    stop_thread(state);
    thread_state_->discard();
  }

  void MachineThread::after_fork_child(STATE) {
    thread_state_ = state->thread_nexus()->create_thread_state(state->machine());
    start(state);
  }
}
