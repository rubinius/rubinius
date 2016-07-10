#include "vm.hpp"
#include "state.hpp"
#include "diagnostics.hpp"
#include "environment.hpp"
#include "logger.hpp"

#include <rapidjson/writer.h>

#include <unistd.h>
#include <fcntl.h>

namespace rubinius {
  namespace diagnostics {
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

    Diagnostics::Diagnostics(STATE)
      : MachineThread(state, "rbx.diagnostics", MachineThread::eSmall)
      , list_()
      , emitter_(NULL)
      , diagnostics_lock_()
      , diagnostics_condition_()
    {
      // TODO: socket target
      if(false /*state->shared().config.system_diagnostics_target.value.compare("none")*/) {
      } else {
        emitter_ = new FileEmitter(state,
            state->shared().config.system_diagnostics_target.value);
      }
    }

    void Diagnostics::initialize(STATE) {
      MachineThread::initialize(state);

      diagnostics_lock_.init();
      diagnostics_condition_.init();
    }

    void Diagnostics::wakeup(STATE) {
      MachineThread::wakeup(state);

      diagnostics_condition_.signal();
    }

    void Diagnostics::after_fork_child(STATE) {
      MachineThread::after_fork_child(state);
    }

    void Diagnostics::report(DiagnosticsData* data) {
      utilities::thread::Mutex::LockGuard guard(diagnostics_lock_);

      list_.push_front(data);

      diagnostics_condition_.signal();
    }

    void Diagnostics::run(STATE) {
      state->vm()->unmanaged_phase(state);

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
        if(data) emitter_->send_diagnostics(data);
      }
    }
  }
}
