#ifndef RBX_MACHINE_H
#define RBX_MACHINE_H

namespace rubinius {
  class ngLogger { };
  class Environment;
  class Configuration;
  class ngDiagnostics { };
  class ThreadNexus;
  class MachineThreads;
  class Memory;

  namespace memory {
    class Collector;
  }

  class ngSignals { };
  class ngCodeDB { };

  class C_API;

  class ngCompiler { };
  class ngDebugger { };
  class ngProfiler { };

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
    ngDiagnostics* _diagnostics_;
    MachineThreads* _machine_threads_;
    Memory* _memory_;
    memory::Collector* _collector_;
    ngSignals* _signals_;
    ngCodeDB* _codedb_;
    C_API* _c_api_;
    ngCompiler* _compiler_;
    ngDebugger* _debugger_;
    ngProfiler* _profiler_;
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

    MachineThreads* const machine_threads() {
      return _machine_threads_;
    }

    Memory* const memory() {
      return _memory_;
    }

    memory::Collector* const collector() {
      return _collector_;
    }

    C_API* const c_api() {
      return _c_api_;
    }

    console::Console* const console() {
      return _console_;
    }

    void boot();
    int halt();

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
