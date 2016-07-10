#include "vm.hpp"
#include "state.hpp"
#include "jit.hpp"
#include "environment.hpp"
#include "logger.hpp"

namespace rubinius {
  namespace jit {
    JIT::JIT(STATE)
      : MachineThread(state, "rbx.jit", MachineThread::eXLarge)
      , list_()
      , jit_lock_()
      , jit_condition_()
    {
    }

    void JIT::initialize(STATE) {
      MachineThread::initialize(state);

      jit_lock_.init();
      jit_condition_.init();
    }

    void JIT::wakeup(STATE) {
      MachineThread::wakeup(state);

      jit_condition_.signal();
    }

    void JIT::after_fork_child(STATE) {
      MachineThread::after_fork_child(state);

      list_.clear();
    }

    void JIT::run(STATE) {
      state->vm()->unmanaged_phase(state);

      while(!thread_exit_) {
        CompileRequest* request = 0;

        {
          utilities::thread::Mutex::LockGuard guard(jit_lock_);

          if(list_.empty()) {
            jit_condition_.wait(jit_lock_);
          } else {
            request = list_.back();
            list_.pop_back();
          }
        }
      }
    }
  }
}
