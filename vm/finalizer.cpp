#include "config.h"
#include "vm.hpp"
#include "finalizer.hpp"
#include "on_stack.hpp"
#include "objectmemory.hpp"

#include "builtin/module.hpp"
#include "builtin/array.hpp"

#include "builtin/array.hpp"
#include "builtin/module.hpp"
#include "builtin/class.hpp"

#include "capi/handle.hpp"

#include "builtin/thread.hpp"

namespace rubinius {

  Object* finalizer_handler_tramp(STATE) {
    state->shared().finalizer_handler()->perform(state);
    return cNil;
  }

  FinalizerHandler::FinalizerHandler(STATE)
    : AuxiliaryThread()
    , shared_(state->shared())
    , target_(state->vm())
    , self_(NULL)
    , thread_(state)
    , exit_(false)
  {
    shared_.auxiliary_threads()->register_thread(this);
    shared_.set_finalizer_handler(this);

    queue_guard_.init();
    lock_.init();
    cond_.init();

    start_thread(state);
  }

  FinalizerHandler::~FinalizerHandler() {
    shared_.auxiliary_threads()->unregister_thread(this);
  }

  void FinalizerHandler::start_thread(STATE) {
    SYNC(state);
    if(self_) return;
    self_ = state->shared().new_vm();
    thread_.set(Thread::create(state, self_, G(thread), finalizer_handler_tramp, false, true));
    run(state);
  }

  void FinalizerHandler::stop_thread(STATE) {
    SYNC(state);
    if(!self_) return;

    // Thread might have already been stopped
    pthread_t os = self_->os_thread();
    exit_ = true;

    cond_.signal();
    void* return_value;
    pthread_join(os, &return_value);
    self_ = NULL;
  }

  void FinalizerHandler::shutdown(STATE) {
    stop_thread(state);
  }

  void FinalizerHandler::before_exec(STATE) {
    stop_thread(state);
  }

  void FinalizerHandler::after_exec(STATE) {
    exit_ = false;
    start_thread(state);
  }

  void FinalizerHandler::before_fork(STATE) {
    stop_thread(state);
  }

  void FinalizerHandler::after_fork_parent(STATE) {
    exit_ = false;
    start_thread(state);
  }

  void FinalizerHandler::after_fork_child(STATE) {
    exit_ = false;
    cond_.init();
    start_thread(state);
  }

  void FinalizerHandler::run(STATE) {
    int error = thread_.get()->fork_attached(state);
    if(error) rubinius::bug("Unable to start finalizer handler thread");
  }

  void FinalizerHandler::perform(STATE) {
    GCTokenImpl gct;
    utilities::thread::Thread::set_os_name("rbx.finalizer");

    state->vm()->thread->hard_unlock(state, gct);

    while(!exit_) {
      FinalizeObject* fi = 0;

      // Take the lock, remove the first one from the list,
      // then process it.
      {
        utilities::thread::Mutex::LockGuard lg(queue_guard_);

        if(!queue_.empty()) {
          fi = queue_.front();
          queue_.pop_front();
        }
      }

      state->vm()->set_call_frame(0);

      if(!fi) {
        utilities::thread::Mutex::LockGuard lg(lock_);
        GCIndependent indy(state);
        cond_.wait(lock_);
        if(exit_) return;
        continue;
      }

      if(fi->ruby_finalizer) {
        CallFrame* call_frame = 0;

        // Rubinius specific code. If the finalizer is cTrue, then
        // send the object the finalize message
        if(fi->ruby_finalizer == cTrue) {
          fi->object->send(state, call_frame, state->symbol("__finalize__"));
        } else {
          Array* ary = Array::create(state, 1);
          ary->set(state, 0, fi->object->id(state));
          fi->ruby_finalizer->send(state, call_frame, G(sym_call), ary);
        }
      }

      if(fi->finalizer) {
        (*fi->finalizer)(state, fi->object);
      }
      // Unhook any handle used by fi->object so that we don't accidentally
      // try and mark it later (after we've finalized it)
      if(capi::Handle* handle = fi->object->handle(state)) {
        handle->forget_object();
        fi->object->clear_handle(state);
      }

      // If the object was remembered, unremember it.
      if(fi->object->remembered_p()) {
        state->memory()->unremember_object(fi->object);
      }

      fi->status = FinalizeObject::eFinalized;
    }
  }

  void FinalizerHandler::schedule(FinalizeObject* fi) {
    utilities::thread::Mutex::LockGuard lg(queue_guard_);
    queue_.push_back(fi);
    cond_.signal();
  }

  void FinalizerHandler::signal() {
    cond_.signal();
  }
}
