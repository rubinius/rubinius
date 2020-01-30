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

  const uint32_t State::hash_seed() {
    return machine()->machine_state()->hash_seed();
  }

  MachineState* const State::machine_state() {
    return vm_->machine()->machine_state();
  }

  Machine* const State::machine() {
    return vm_->machine();
  }

  Configuration* const State::configuration() {
    return machine()->configuration();
  }

  Environment* const State::environment() {
    return machine()->environment();
  }

  ThreadNexus* const State::thread_nexus() {
    return machine()->thread_nexus();
  }

  Diagnostics* const State::diagnostics() {
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

  Profiler* const State::profiler() {
    return machine()->profiler();
  }

  Console* const State::console() {
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
