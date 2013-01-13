#ifndef RBX_FINALIZER_HPP
#define RBX_FINALIZER_HPP

#include "lock.hpp"
#include "auxiliary_threads.hpp"

#include "gc/finalize.hpp"
#include "gc/root.hpp"

#include <list>

namespace rubinius {
  class VM;
  class State;
  struct CallFrame;
  class Thread;

  Object* handle_tramp(STATE);

  class FinalizerHandler : public AuxiliaryThread, public Lockable {
    SharedState& shared_;
    VM* target_;
    VM* self_;

    TypedRoot<Thread*> thread_;
    std::list<FinalizeObject*> queue_;
    utilities::thread::Mutex queue_guard_;
    utilities::thread::Mutex lock_;
    utilities::thread::Condition cond_;
    bool exit_;

  public:

    FinalizerHandler(STATE);
    virtual ~FinalizerHandler();

    void perform(State*);

    void schedule(FinalizeObject* fi);
    void signal();

    void start_thread(STATE);
    void stop_thread(STATE);

    void shutdown(STATE);
    void before_exec(STATE);
    void after_exec(STATE);
    void before_fork(STATE);
    void after_fork_parent(STATE);
    void after_fork_child(STATE);

    void run(State*);
  };
}

#endif

