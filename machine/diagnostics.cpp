#include "vm.hpp"
#include "state.hpp"
#include "environment.hpp"
#include "logger.hpp"

#include "diagnostics.hpp"
#include "diagnostics/emitter.hpp"
#include "diagnostics/measurement.hpp"

#include <unistd.h>
#include <fcntl.h>

namespace rubinius {
  using namespace utilities;

  namespace diagnostics {
    Measurement* Measurement::create_counter(STATE) {
      Measurement* m = new Measurement();

      m->update(Measurement::update_counter);
      m->report(Measurement::report_counter);

      state->shared().diagnostics()->add_measurement(m);

      return m;
    }

    Reporter::Reporter(STATE, Diagnostics* d)
      : MachineThread(state, "rbx.diagnostics", MachineThread::eSmall)
      , timer_(nullptr)
      , interval_(state->shared().config.diagnostics_interval)
      , list_()
      , emitter_(Emitter::create(state))
      , diagnostics_lock_()
      , diagnostics_(d)
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

    void Reporter::report(Formatter* formatter) {
      std::lock_guard<std::mutex> guard(diagnostics_lock_);

      list_.push_back(formatter);

      if(timer_) {
        timer_->clear();
        timer_->cancel();
      }
    }

    void Reporter::run(STATE) {
      state->vm()->unmanaged_phase(state);

      while(!thread_exit_) {
        timer_->set(interval_);

        if(timer_->wait_for_tick() < 0) {
          logger::error("diagnostics: error waiting for timer");
        }

        if(thread_exit_) break;

        {
          std::lock_guard<std::mutex> guard(diagnostics_lock_);

          for(auto m : diagnostics_->measurements()) {
            m->report(state);
          }

          if(!list_.empty()) {
            Formatter* formatter = list_.back();
            list_.pop_back();

            emitter_->transmit(formatter);
          }
        }
      }
    }
  }
}
