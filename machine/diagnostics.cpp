#include "configuration.hpp"
#include "thread_state.hpp"
#include "environment.hpp"
#include "logger.hpp"
#include "thread_phase.hpp"

#include "diagnostics.hpp"
#include "diagnostics/codedb.hpp"
#include "diagnostics/collector.hpp"
#include "diagnostics/diagnostic.hpp"
#include "diagnostics/emitter.hpp"
#include "diagnostics/machine.hpp"
#include "diagnostics/memory.hpp"

#include <unistd.h>
#include <fcntl.h>

namespace rubinius {
  using namespace utilities;

  namespace diagnostics {
    Reporter::Reporter(STATE, Diagnostics* d)
      : MachineThread(state, "rbx.diagnostics", MachineThread::eSmall)
      , diagnostics_(d)
      , timer_(nullptr)
      , emitter_(Emitter::create(state))
    {
    }

    void Reporter::initialize(STATE) {
      MachineThread::initialize(state);

      timer_ = new timer::Timer;
    }

    void Reporter::wakeup(STATE) {
      MachineThread::wakeup(state);

      if(timer_) {
        timer_->clear();
        timer_->cancel();
      }
    }

    void Reporter::after_fork_child(STATE) {
      MachineThread::after_fork_child(state);
    }

    void Reporter::report() {
      if(timer_) {
        timer_->clear();
        timer_->cancel();
      }
    }

    void Reporter::run(STATE) {
      state->unmanaged_phase();

      while(!thread_exit_) {
        timer_->set(diagnostics_->interval());

        if(timer_->wait_for_tick() < 0) {
          logger::error("diagnostics: error waiting for timer");
        }

        {
          std::lock_guard<std::mutex> guard(diagnostics_->lock());

          while(!diagnostics_->intermittent_reports().empty()) {
            auto f = diagnostics_->intermittent_reports().back();
            diagnostics_->intermittent_reports().pop_back();

            emitter_->transmit(f);
          }
        }

        if(timer_->canceled_p()) continue;

        {
          ManagedPhase managed(state);
          LockWaiting<std::mutex> lock_waiting(state, diagnostics_->lock());

          for(auto f : diagnostics_->recurring_reports()) {
            f->update();
          }
        }

        {
          std::lock_guard<std::mutex> guard(diagnostics_->lock());

          for(auto f : diagnostics_->recurring_reports()) {
            emitter_->transmit(f);
          }
        }
      }
    }
  }

  Diagnostics::Diagnostics(Configuration* configuration)
    : boot_metrics_(new diagnostics::BootMetrics())
    , codedb_metrics_(new diagnostics::CodeDBMetrics())
    , collector_metrics_(new diagnostics::CollectorMetrics())
    , memory_metrics_(new diagnostics::MemoryMetrics())
    , recurring_reports_()
    , intermittent_reports_()
    , reporter_(nullptr)
    , lock_()
    , interval_(configuration->diagnostics_interval)
  {
  }
}
