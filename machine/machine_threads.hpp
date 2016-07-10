#ifndef RBX_AUXILIARY_THREADS_H
#define RBX_AUXILIARY_THREADS_H

#include "defines.hpp"

#include "util/thread.hpp"

#include <atomic>
#include <string>
#include <list>

namespace rubinius {
  class MachineThread;
  class VM;

  typedef std::list<MachineThread*> MachineThreadList;

  class MachineThread {
    VM* vm_;
    uint32_t stack_size_;

  protected:

    std::atomic<bool> thread_running_;
    std::atomic<bool> thread_exit_;

  public:

    enum StackSize {
      eSmall  = 0x10000,
      eLarge  = 0x100000,
      eXLarge = 0x400000,
    };

    MachineThread(STATE, std::string name, StackSize stack_size=eLarge);
    virtual ~MachineThread() { };

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

    bool thread_running_p() {
      return thread_running_;
    }

    virtual void initialize(STATE);
    virtual void start(STATE);
    virtual void start_thread(STATE);
    virtual void wakeup(STATE);
    virtual void stop_thread(STATE);
    virtual void stop(STATE);
  };

  class MachineThreads {
  private:
    bool fork_in_progress_;
    bool exec_in_progress_;
    bool fork_exec_in_progress_;
    bool shutdown_in_progress_;
    utilities::thread::Mutex mutex_;
    MachineThreadList threads_;

  public:
    MachineThreads()
      : fork_in_progress_(false)
      , exec_in_progress_(false)
      , fork_exec_in_progress_(false)
      , shutdown_in_progress_(false)
      , mutex_()
      , threads_()
    {
    }

    void register_thread(MachineThread* thread);
    void unregister_thread(MachineThread* thread);

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
