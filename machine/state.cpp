#include "call_frame.hpp"
#include "machine.hpp"
#include "helpers.hpp"
#include "memory.hpp"
#include "park.hpp"
#include "signal.hpp"
#include "state.hpp"
#include "vm.hpp"

#include "class/exception.hpp"
#include "class/location.hpp"
#include "class/array.hpp"
#include "class/fiber.hpp"

namespace rubinius {
  void State::raise_stack_error() {
    Class* stack_error = globals().stack_error.get();
    Exception* exc = memory()->new_object<Exception>(this, stack_error);
    exc->locations(this, Location::from_call_stack(this));
    vm_->thread_state()->raise_exception(exc);
  }

  Object* State::park(STATE) {
    return vm_->park_->park(this);
  }

  Object* State::park_timed(STATE, struct timespec* ts) {
    return vm_->park_->park_timed(this, ts);
  }

  Machine* const State::machine() {
    return shared_.machine();
  }

  Configuration* const State::configuration() {
    return machine()->configuration();
  }

  ThreadNexus* const State::thread_nexus() {
    return machine()->thread_nexus();
  }

  diagnostics::Diagnostics* const diagnostics() {
    return machine()->diagnostics();
  }

  MachineThreads* const State::machine_threads() {
    return machine()->machine_threads();
  }

  memory::Collector* const State::collector() {
    return machine()->collector();
  }

  SignalThread* const State::signals() {
    return machine()->signals();
  }

  Memory* const State::memory() {
    return machine()->memory();
  }

  C_API* const State::c_api() {
    return machine()->c_api();
  }

  diagnostics::Profiler* const profiler() {
    return machine()->profiler();
  }

  Console* const console() {
    return machine()->console();
  }

  Globals& State::globals() {
    return memory()->globals;
  }

  Symbol* const State::symbol(const char* str) {
    return memory()->symbols.lookup(this, str, strlen(str));
  }

  Symbol* const State::symbol(const char* str, size_t len) {
    return memory()->symbols.lookup(this, str, len);
  }

  Symbol* const State::symbol(std::string str) {
    return memory()->symbols.lookup(this, str);
  }

  Symbol* const State::symbol(String* str) {
    return memory()->symbols.lookup(this, str);
  }
}
