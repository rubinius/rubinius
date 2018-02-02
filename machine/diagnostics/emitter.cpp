#include "configuration.hpp"
#include "environment.hpp"
#include "shared_state.hpp"
#include "state.hpp"
#include "vm.hpp"

#include "diagnostics.hpp"
#include "diagnostics/diagnostic.hpp"
#include "diagnostics/emitter.hpp"

#include <fcntl.h>

namespace rubinius {
  namespace diagnostics {
    Emitter* Emitter::create(STATE) {
      // TODO: socket target
      if(false /*state->shared().config.diagnostics_target.value.compare("none")*/) {
      } else {
        return new FileEmitter(state, state->shared().config.diagnostics_target.value);
      }
    }

    FileEmitter::FileEmitter(STATE, std::string path)
      : Emitter()
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

    void FileEmitter::transmit(Diagnostic* formatter) {
      std::string str = formatter->to_string().c_str();

      if(::write(fd_, str.c_str(), str.size()) < str.size()) {
        logger::error("%s: writing diagnostics failed", strerror(errno));
      }
    }
  }
}
