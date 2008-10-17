#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/list.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/contexts.hpp"
#include "builtin/channel.hpp"

#include "objectmemory.hpp"
#include "vm.hpp"

namespace rubinius {
  void Thread::init(STATE) {
    Tuple* tup = Tuple::from(state, 7, List::create(state), List::create(state),
        List::create(state), List::create(state), List::create(state),
        List::create(state), List::create(state));

    GO(scheduled_threads).set(tup);

    GO(thread).set(state->new_class("Thread", G(object), Thread::fields));
    G(thread)->set_object_type(state, Thread::type);

    G(thread)->set_const(state, "ScheduledThreads", tup);
  }

  Thread* Thread::create(STATE) {
    Thread* thr = (Thread*)state->new_object(G(thread));
    thr->priority(state, Fixnum::from(2));

    thr->boot_task(state);
    thr->sleep(state, Qtrue);
    thr->alive(state, Qtrue);

    state->interrupts.use_preempt = true;
    state->interrupts.enable_preempt = true;

    return thr;
  }

  Thread* Thread::current(STATE) {
    return state->current_thread();
  }

  void Thread::boot_task(STATE) {
    Task* task = Task::create(state);
    this->task(state, task);
  }

  Object* Thread::pass(STATE) {
    Thread* current = Thread::current(state);

    state->queue_thread(current);
    state->run_best_thread();

    return Qnil;
  }

  Thread* Thread::run(STATE) {
    state->queue_thread(Thread::current(state));

    state->activate_thread(this);

    return this;
  }

  // Called when VM is putting this Thread back into play. It
  // doesn't mean it's about to run, just that it's scheduled
  // to do so.
  void Thread::woken(STATE) {
    sleep(state, Qfalse);
    channel(state, (Channel*)Qnil);
  }

  void Thread::set_top(STATE, OBJECT val) {
    task_->active()->set_top(val);
  }

  void Thread::sleep_for(STATE, Channel* chan) {
    channel(state, chan);
    sleep(state, Qtrue);
  }

  Thread* Thread::wakeup(STATE) {
    state->queue_thread(this);
    return this;
  }

  OBJECT Thread::raise(STATE, Exception* exc) {
    wakeup(state);
    MethodContext* ctx = task_->active();
    ctx->reference(state);
    exc->context(state, ctx);
    return task_->raise(state, exc);
  }

  ExecuteStatus Thread::dequeue_prim(STATE, Executable* exec, Task* task, Message& msg) {
    alive(state, Qfalse);
    task_ = (Task*)Qnil;

    // TODO make sure this isn't in global.scheduled_threads

    // TODO clear the channel's events rather than making sure there isn't one
    assert(channel()->nil_p());

    state->check_events();
    return cExecuteRestart;
  }

}
