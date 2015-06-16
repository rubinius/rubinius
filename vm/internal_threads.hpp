#ifndef RBX_AUXILIARY_THREADS_H
#define RBX_AUXILIARY_THREADS_H

#include "lock.hpp"

#include "util/thread.hpp"

#include <string>
#include <list>

namespace rubinius {
  class VM;

  namespace metrics {
    struct MetricsData;
  }

  class InternalThread {
    VM* vm_;
    bool thread_running_;
    uint32_t stack_size_;

    metrics::MetricsData& metrics_;

  protected:

    bool thread_exit_;

  public:

    enum StackSize {
      eSmall  = 0x1000,
      eLarge  = 0x10000,
      eXLarge = 0x100000,
    };

    InternalThread(STATE, std::string name, StackSize stack_size=eLarge);
    virtual ~InternalThread() { };

    // OS thread trampoline
    static void* run(void*);

    // Events
    virtual void before_exec(STATE) { };
    virtual void after_exec(STATE) { };
    virtual void before_fork_exec(STATE) { };
    virtual void after_fork_exec_parent(STATE) { };
    virtual void after_fork_exec_child(STATE) { };
    virtual void before_fork(STATE) { };
    virtual void after_fork_parent(STATE) { };
    virtual void after_fork_child(STATE);
    virtual void run(STATE) { };

    // Object interface
    VM* vm() {
      return vm_;
    }

    metrics::MetricsData& metrics() {
      return metrics_;
    }

    virtual void initialize(STATE);
    virtual void start(STATE);
    virtual void start_thread(STATE);
    virtual void wakeup(STATE);
    virtual void stop_thread(STATE);
    virtual void stop(STATE);
  };

  class InternalThreads {
  private:
    bool fork_in_progress_;
    bool exec_in_progress_;
    bool fork_exec_in_progress_;
    bool shutdown_in_progress_;
    utilities::thread::Mutex mutex_;
    std::list<InternalThread*> threads_;

  public:
    InternalThreads()
      : fork_in_progress_(false)
      , exec_in_progress_(false)
      , fork_exec_in_progress_(false)
      , shutdown_in_progress_(false)
    {
    }

    void register_thread(InternalThread* thread);
    void unregister_thread(InternalThread* thread);

    void init();
    void shutdown(STATE);
    void before_exec(STATE);
    void after_exec(STATE);
    void before_fork_exec(STATE);
    void after_fork_exec_parent(STATE);
    void after_fork_exec_child(STATE);
    void before_fork(STATE);
    void after_fork_parent(STATE);
    void after_fork_child(STATE);
  };
}

#endif
