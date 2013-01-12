#include "config.h"
#include "vm.hpp"
#include "on_stack.hpp"
#include "objectmemory.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/thread.hpp"

#include "capi/handle.hpp"

#include "gc/finalize.hpp"

namespace rubinius {
  FinalizerHandler::iterator::iterator(FinalizerHandler* fh)
    : handler_(fh)
    , current_list_(NULL)
  {
    current_list_ = handler_->live_list_;
    current_ = current_list_->begin();

    if(handler_->lists_->empty()) {
      end_ = current_list_->end();
    } else {
      lists_iterator_ = handler_->lists_->begin();
      end_ = handler_->lists_->back()->end();
    }
  }

  void FinalizerHandler::iterator::next(bool live) {
    if(current_list_ == handler_->live_list_) {
      if(live) {
        ++current_;
      } else {
        handler_->dead_list_->push_back(*current_);
        current_ = current_list_->erase(current_);
      }

      if(current_ == current_list_->end()) {
        if(!handler_->lists_->empty()) {
          current_list_ = *lists_iterator_;
          current_ = current_list_->begin();
        }
      }
    } else {
      if(++current_ == end_) {
        return;
      } else if(current_ == current_list_->end()) {
        if(++lists_iterator_ != handler_->lists_->end()) {
          current_list_ = *lists_iterator_;
          current_ = current_list_->begin();
        }
      }
    }
  }

  bool FinalizerHandler::iterator::end() {
    return current_ == end_;
  }

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
    , lists_(NULL)
    , live_list_(NULL)
    , dead_list_(NULL)
    , process_list_(NULL)
    , iterator_(NULL)
    , process_item_kind_(eRuby)
    , exit_(false)
    , finishing_(false)
  {
    shared_.auxiliary_threads()->register_thread(this);
    shared_.set_finalizer_handler(this);

    lists_ = new FinalizeObjectsList();
    live_list_ = new FinalizeObjects();

    live_guard_.init();
    lock_.init();
    cond_.init();
  }

  FinalizerHandler::~FinalizerHandler() {
    if(self_) {
      rubinius::bug("deleting FinalizerHandler with worker thread active");
    }

    shared_.auxiliary_threads()->unregister_thread(this);

    if(iterator_) delete iterator_;
    if(process_list_) delete process_list_;
    if(live_list_) delete live_list_;
    if(dead_list_) delete dead_list_;
    if(lists_) delete lists_;
  }

  void FinalizerHandler::start_thread(STATE) {
    SYNC(state);
    if(self_) return;
    self_ = state->shared().new_vm();
    exit_ = false;
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
    start_thread(state);
  }

  void FinalizerHandler::before_fork(STATE) {
    stop_thread(state);
  }

  void FinalizerHandler::after_fork_parent(STATE) {
    start_thread(state);
  }

  void FinalizerHandler::after_fork_child(STATE) {
    live_guard_.init();
    lock_.init();
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
      if(!process_list_) first_process_item();

      if(!process_list_) {
        utilities::thread::Mutex::LockGuard lg(lock_);
        GCIndependent indy(state);
        cond_.wait(lock_);
        continue;
      }

      finalize(state);
      next_process_item();
    }
  }

  void FinalizerHandler::finalize(STATE) {
    state->vm()->set_call_frame(0);

    switch(process_item_kind_) {
    case eRuby: {
      CallFrame* call_frame = 0;

      if(process_item_->ruby_finalizer) {
        // Rubinius specific code. If the finalizer is cTrue, then send the
        // object the __finalize__ message.
        if(process_item_->ruby_finalizer == cTrue) {
          process_item_->object->send(state, call_frame, state->symbol("__finalize__"));
        } else {
          Array* ary = Array::create(state, 1);
          ary->set(state, 0, process_item_->object->id(state));
          process_item_->ruby_finalizer->send(state, call_frame, G(sym_call), ary);
        }
      }

      break;
    }
    case eNative:
      if(process_item_->finalizer) {
        (*process_item_->finalizer)(state, process_item_->object);
      }
      break;
    case eRelease:
      // Unhook any handle used by fi->object so that we don't accidentally
      // try and mark it later (after we've finalized it)
      if(capi::Handle* handle = process_item_->object->handle(state)) {
        handle->forget_object();
        process_item_->object->clear_handle(state);
      }

      // If the object was remembered, unremember it.
      if(process_item_->object->remembered_p()) {
        state->memory()->unremember_object(process_item_->object);
      }

      process_item_->status = FinalizeObject::eFinalized;

      break;
    }
  }

  void FinalizerHandler::first_process_item() {
    if(!process_list_ && !lists_->empty()) {
      process_list_ = lists_->back();
      process_item_ = process_list_->begin();
    }
  }

  void FinalizerHandler::next_process_item(bool release) {
    if(++process_item_ == process_list_->end()) {
      switch(process_item_kind_) {
      case eRuby:
        process_item_ = process_list_->begin();
        process_item_kind_ = eNative;
        break;
      case eNative:
        process_item_ = process_list_->begin();
        process_item_kind_ = eRelease;
        // No need to release objects when e.g. shutting down, so we only
        // break here if we are going to release objects.
        if(release) break;
      case eRelease:
        delete process_list_;
        process_list_ = NULL;
        process_item_kind_ = eRuby;
        lists_->pop_back();
        break;
      }
    }
  }

  void FinalizerHandler::finish(STATE) {
    stop_thread(state);
    if(self_) rubinius::bug("FinalizerHandler worker thread active after stop_thread");

    finishing_ = true;

    if(!live_list_->empty()) lists_->push_front(live_list_);

    while(!lists_->empty()) {
      first_process_item();
      while(process_list_) {
        finalize(state);
        next_process_item(false);
      }
    }
  }

  void FinalizerHandler::record(Object* obj, FinalizerFunction func) {
    utilities::thread::Mutex::LockGuard lg(live_guard_);

    if(finishing_) {
      rubinius::bug("FinalizerHandler::record called when finishing finalizers for halt");
    }

    FinalizeObject fi;
    fi.object = obj;
    fi.status = FinalizeObject::eLive;
    fi.finalizer = func;

    // Makes a copy of fi.
    live_list_->push_front(fi);
  }

  void FinalizerHandler::set_ruby_finalizer(Object* obj, Object* finalizer) {
    utilities::thread::Mutex::LockGuard lg(live_guard_);

    // Check if the object is already in the finalizer list.
    for(FinalizeObjects::iterator i = live_list_->begin();
        i != live_list_->end();
        ++i)
    {
      if(i->object == obj) {
        if(finalizer->nil_p()) {
          live_list_->erase(i);
        } else {
          i->ruby_finalizer = finalizer;
        }
        return;
      }
    }

    // Adding a nil finalizer is only used to delete an existing finalizer,
    // which we apparently don't have if we get here.
    if(finalizer->nil_p()) {
      return;
    }

    // Ok, create it.

    FinalizeObject fi;
    fi.object = obj;
    fi.status = FinalizeObject::eLive;

    // Rubinius specific API. If the finalizer is the object, we're going to send
    // the object __finalize__. We mark that the user wants this by putting cTrue
    // as the ruby_finalizer.
    if(obj == finalizer) {
      fi.ruby_finalizer = cTrue;
    } else {
      fi.ruby_finalizer = finalizer;
    }

    // Makes a copy of fi.
    live_list_->push_front(fi);
  }

  void FinalizerHandler::start_collection(STATE) {
    if(process_item_kind_ == eRelease) {
      while(process_list_) {
        finalize(state);
        next_process_item();
      }
    }

    if(!dead_list_) dead_list_ = new FinalizeObjects();
  }

  void FinalizerHandler::finish_collection(STATE) {
    if(!dead_list_->empty()) {
      lists_->push_front(dead_list_);
      dead_list_ = NULL;
    }

    if(iterator_) {
      delete iterator_;
      iterator_ = NULL;
    }

    signal();
  }

  void FinalizerHandler::signal() {
    cond_.signal();
  }

  FinalizerHandler::iterator& FinalizerHandler::begin() {
    if(iterator_) delete iterator_;
    iterator_ = new iterator(this);
    return *iterator_;
  }
}
