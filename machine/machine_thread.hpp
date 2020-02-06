#ifndef RBX_AUXILIARY_THREADS_H
#define RBX_AUXILIARY_THREADS_H

#include "defines.hpp"

#include <atomic>
#include <string>
#include <list>

namespace rubinius {
  class MachineThread;
  class ThreadState;

  class MachineThread {
    ThreadState* thread_state_;
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
    virtual void before_fork(STATE) { };
    virtual void after_fork_parent(STATE) { };
    virtual void after_fork_child(STATE);
    virtual void run(STATE) { };

    // Object interface
    ThreadState* thread_state() {
      return thread_state_;
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
}

#endif
