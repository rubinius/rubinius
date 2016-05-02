#include "vm.hpp"
#include "state.hpp"
#include "diagnostics.hpp"

namespace rubinius {
  namespace diagnostics {
    Diagnostics::Diagnostics(STATE)
      : InternalThread(state, "rbx.diagnostics", InternalThread::eSmall)
      , list_()
      , diagnostics_lock_()
      , diagnostics_condition_()
    {
    }

    void Diagnostics::initialize(STATE) {
      InternalThread::initialize(state);

      diagnostics_lock_.init();
    }

    void Diagnostics::wakeup(STATE) {
      InternalThread::wakeup(state);
    }

    void Diagnostics::after_fork_child(STATE) {
      InternalThread::after_fork_child(state);
    }

    void Diagnostics::run(STATE) {
      state->vm()->become_unmanaged();

      while(!thread_exit_) {
        DiagnosticsData* data = 0;

        {
          utilities::thread::Mutex::LockGuard guard(diagnostics_lock_);

          if(list_.empty()) {
            diagnostics_condition_.wait(diagnostics_lock_);
          } else {
            data = list_.back();
            list_.pop_back();
          }
        }

        // Emit data
      }
    }
  }
}
