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
    : thread_state_(state->threads()->create_thread_state(name.c_str()))
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

    SET_THREAD_UNWIND(state);

    if(!state->thread_unwinding_p()) {
      RUBINIUS_THREAD_START(
          const_cast<RBX_DTRACE_CHAR_P>(state->name().c_str()), state->thread_id(), 1);

      state->set_stack_bounds(thread->stack_size_);

      NativeMethod::init_thread(state);

      thread->thread_running_ = true;

      thread->run(state);
    }

    thread->thread_running_ = false;

    NativeMethod::cleanup_thread(state);

    RUBINIUS_THREAD_STOP(
        const_cast<RBX_DTRACE_CHAR_P>(state->name().c_str()), state->thread_id(), 1);

    state->set_call_frame(nullptr);
    state->unmanaged_phase();

    // There's no way to retain a reference to this, so discard immediately.
    state->threads()->remove_thread_state(state);
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
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

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
  }

  void MachineThread::stop(STATE) {
    stop_thread(state);
    thread_state_->discard();
  }

  void MachineThread::after_fork_child(STATE) {
    start(state);
  }
}
