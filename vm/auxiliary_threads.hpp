#ifndef RBX_AUXILIARY_THREADS_H
#define RBX_AUXILIARY_THREADS_H

#include "util/thread.hpp"

#include <list>

namespace rubinius {
  class AuxiliaryThread {
  public:

    virtual ~AuxiliaryThread() { };
    virtual void shutdown(STATE) { };
    virtual void before_exec(STATE) { };
    virtual void after_exec(STATE) { };
    virtual void before_fork(STATE) { };
    virtual void after_fork_parent(STATE) { };
    virtual void after_fork_child(STATE) { };
  };

  class AuxiliaryThreads {
  private:
    bool fork_in_progress_;
    bool exec_in_progress_;
    bool shutdown_in_progress_;
    utilities::thread::Mutex mutex_;
    std::list<AuxiliaryThread*> threads_;

  public:
    AuxiliaryThreads()
      : fork_in_progress_(false)
      , exec_in_progress_(false)
      , shutdown_in_progress_(false)
    {
    }

    void register_thread(AuxiliaryThread* thread);
    void unregister_thread(AuxiliaryThread* thread);

    void init();
    void shutdown(STATE);
    void before_exec(STATE);
    void after_exec(STATE);
    void before_fork(STATE);
    void after_fork_parent(STATE);
    void after_fork_child(STATE);
  };
}

#endif
