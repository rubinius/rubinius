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
    DiagnosticsData::~DiagnosticsData() {
      if(document_) delete document_;
    }

    void DiagnosticsData::to_string(rapidjson::StringBuffer buffer) {
      rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

      document_->Accept(writer);
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
      size_t size = buffer.GetSize();

      if(::write(fd_, buffer.GetString(), size) < size) {
        logger::error("%s: writing diagnostics failed", strerror(errno));
      }
    }

    Diagnostics::Diagnostics(STATE)
      : InternalThread(state, "rbx.diagnostics", InternalThread::eSmall)
      , list_()
      , emitter_(NULL)
      , diagnostics_lock_()
      , diagnostics_condition_()
    {
      if(state->shared().config.system_diagnostics_target.value.compare("none")) {
        emitter_ = new FileEmitter(state,
            state->shared().config.system_diagnostics_target.value);
      }
    }

    void Diagnostics::initialize(STATE) {
      InternalThread::initialize(state);

      diagnostics_lock_.init();
    }

    void Diagnostics::wakeup(STATE) {
      InternalThread::wakeup(state);

      diagnostics_condition_.signal();
    }

    void Diagnostics::after_fork_child(STATE) {
      InternalThread::after_fork_child(state);
    }

    void Diagnostics::report(DiagnosticsData* data) {
      utilities::thread::Mutex::LockGuard guard(diagnostics_lock_);

      list_.push_front(data);

      diagnostics_condition_.signal();
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
        if(emitter_) emitter_->send_diagnostics(data);
      }
    }
  }
}
