#include "thread_state.hpp"
#include "machine_compiler.hpp"
#include "environment.hpp"
#include "logger.hpp"

namespace rubinius {
  namespace jit {
    MachineCompiler::MachineCompiler(STATE)
      : MachineThread(state, "rbx.jit", MachineThread::eXLarge)
      , list_()
      , list_mutex_()
      , list_condition_()
    {
    }

    void MachineCompiler::initialize(STATE) {
      MachineThread::initialize(state);
    }

    void MachineCompiler::wakeup(STATE) {
      MachineThread::wakeup(state);

      list_condition_.notify_one();
    }

    void MachineCompiler::after_fork_child(STATE) {
      MachineThread::after_fork_child(state);

      list_.clear();
    }

    void MachineCompiler::run(STATE) {
      state->unmanaged_phase(state);

      while(!thread_exit_) {
        CompileRequest* request = 0;

        {
          std::unique_lock<std::mutex> lk(list_mutex_);
          list_condition_.wait(lk,
              [this]{ return thread_exit_ || !list_.empty(); });

          if(!list_.empty()) {
            request = list_.back();
            list_.pop_back();
          }
        }
      }
    }
  }
}
