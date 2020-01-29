#ifndef RBX_MACHINE_H
#define RBX_MACHINE_H

#include "defines.hpp"

namespace rubinius {
  class ngLogger { };
  class Environment;
  class Configuration;

  namespace diagnostics {
    class Diagnostics;
    class Diagnostic;
  }

  class ThreadNexus;
  class MachineThreads;
  class Memory;

  namespace memory {
    class Collector;
  }

  class SignalThread;
  class ngCodeDB { };

  class C_API;

  namespace jit {
    class MachineCompiler;
  }

  class ngDebugger { };

  namespace diagnostics {
    class Profiler;
  }

  namespace console {
    class Console;
  }

  class MachineState {
  };

  class Machine {
    MachineState* _machine_state_;
    ngLogger* _logger_;
    ThreadNexus* _thread_nexus_;
    Configuration* _configuration_;
    Environment* _environment_;
    diagnostics::Diagnostics* _diagnostics_;
    MachineThreads* _machine_threads_;
    Memory* _memory_;
    memory::Collector* _collector_;
    SignalThread* _signals_;
    ngCodeDB* _codedb_;
    C_API* _c_api_;
    jit::MachineCompiler* _compiler_;
    ngDebugger* _debugger_;
    diagnostics::Profiler* _profiler_;
    console::Console* _console_;

  public:

    Machine(int argc, char** argv);
    virtual ~Machine();

    ThreadNexus* const thread_nexus() {
      return _thread_nexus_;
    }

    Configuration* const configuration() {
      return _configuration_;
    }

    Environment* const environment() const {
      return _environment_;
    }

    diagnostics::Diagnostics* const diagnostics() {
      return _diagnostics_;
    }

    MachineThreads* const machine_threads() {
      return _machine_threads_;
    }

    Memory* const memory() {
      return _memory_;
    }

    memory::Collector* const collector() {
      return _collector_;
    }

    SignalThread* const signals() {
      return _signals_;
    }

    C_API* const c_api() {
      return _c_api_;
    }

    jit::MachineCompiler* const compiler() {
      return _compiler_;
    }

    diagnostics::Profiler* const profiler() {
      return _profiler_;
    }

    console::Console* const console() {
      return _console_;
    }

    void boot();
    int halt();

    SignalThread* start_signals(STATE);
    diagnostics::Diagnostics* start_diagnostics(STATE);
    void report_diagnostics(diagnostics::Diagnostic* diagnostic);

    void halt_console();
    void halt_profiler();
    void halt_debugger();
    void halt_compiler();
    void halt_c_api();
    void halt_codedb();
    void halt_signals();
    void halt_collector();
    void halt_memory();
    void halt_machine_threads();
    void halt_thread_nexus();
    void halt_diagnostics();
    void halt_configuration();
    void halt_environment();
    void halt_logger();
    void halt_machine_state();
  };
}
#endif
