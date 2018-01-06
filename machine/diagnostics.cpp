#include "vm.hpp"
#include "state.hpp"
#include "diagnostics.hpp"
#include "environment.hpp"
#include "logger.hpp"

#include <rapidjson/writer.h>

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

    DiagnosticsData::DiagnosticsData()
      : document()
    {
      document.SetObject();

      document.AddMember("diagnostic_type", "DiagnosticsData", document.GetAllocator());
    }

    void DiagnosticsData::set_type(const char* type) {
      document["diagnostic_type"].SetString(type, document.GetAllocator());
    }

    void DiagnosticsData::to_string(rapidjson::StringBuffer& buffer) {
      rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

      document.Accept(writer);
    }

    MemoryDiagnostics::MemoryDiagnostics()
      : DiagnosticsData()
      , objects_(0)
      , bytes_(0)
    {
      set_type("MemoryDiagnostics");

      document.AddMember("objects", objects_, document.GetAllocator());
      document.AddMember("bytes", bytes_, document.GetAllocator());
    }

    FileEmitter::FileEmitter(STATE, std::string path)
      : DiagnosticsEmitter()
      , path_(path)
      , fd_(-1)
    {
      // TODO: Make this a proper feature of the config facility.
      state->shared().env()->expand_config_value(
          path_, "$PID", state->shared().pid.c_str());

      if((fd_ = ::open(path_.c_str(), O_CREAT | O_WRONLY | O_CLOEXEC, 0660)) < 0) {
        logger::error("%s: unable to open diagnostics file: %s",
            strerror(errno), path_.c_str());
      }
    }

    void FileEmitter::send_diagnostics(DiagnosticsData* data) {
      rapidjson::StringBuffer buffer;

      data->to_string(buffer);

      ssize_t size = buffer.GetSize();

      if(::write(fd_, buffer.GetString(), size) < size) {
        logger::error("%s: writing diagnostics failed", strerror(errno));
      }
    }

    DiagnosticsReporter::DiagnosticsReporter(STATE, Diagnostics* d)
      : MachineThread(state, "rbx.diagnostics", MachineThread::eSmall)
      , timer_(NULL)
      , interval_(state->shared().config.system_diagnostics_interval)
      , list_()
      , emitter_(NULL)
      , diagnostics_lock_()
      , diagnostics_(d)
    {
      // TODO: socket target
      if(false /*state->shared().config.system_diagnostics_target.value.compare("none")*/) {
      } else {
        emitter_ = new FileEmitter(state,
            state->shared().config.system_diagnostics_target.value);
      }
    }

    void DiagnosticsReporter::initialize(STATE) {
      MachineThread::initialize(state);

      timer_ = new timer::Timer;

      diagnostics_lock_.init();
    }

    void DiagnosticsReporter::wakeup(STATE) {
      MachineThread::wakeup(state);

      if(timer_) {
        timer_->clear();
        timer_->cancel();
      }
    }

    void DiagnosticsReporter::after_fork_child(STATE) {
      MachineThread::after_fork_child(state);
    }

    void DiagnosticsReporter::report(DiagnosticsData* data) {
      utilities::thread::Mutex::LockGuard guard(diagnostics_lock_);

      list_.push_front(data);

      if(timer_) {
        timer_->clear();
        timer_->cancel();
      }
    }

    void DiagnosticsReporter::run(STATE) {
      state->vm()->unmanaged_phase(state);

      timer_->set(interval_);

      while(!thread_exit_) {
        DiagnosticsData* data = 0;

        if(timer_->wait_for_tick() < 0) {
          logger::error("diagnostics: error waiting for timer");
        }

        if(thread_exit_) break;

        {
          utilities::thread::Mutex::LockGuard guard(diagnostics_lock_);

          if(!list_.empty()) {
            data = list_.back();
            list_.pop_back();
          }

          for(auto m : diagnostics_->measurements()) {
            m->report(state);
          }
        }

        // Emit data
        if(data) emitter_->send_diagnostics(data);
      }
    }
  }
}
