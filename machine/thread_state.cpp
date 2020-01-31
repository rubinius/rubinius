#include "call_frame.hpp"
#include "machine.hpp"
#include "helpers.hpp"
#include "memory.hpp"
#include "park.hpp"
#include "signal.hpp"
#include "thread_state.hpp"
#include "vm.hpp"

#include "class/exception.hpp"
#include "class/location.hpp"
#include "class/array.hpp"
#include "class/fiber.hpp"
#include "class/unwind_state.hpp"

namespace rubinius {
  Object* ThreadState::raise_exception(Exception* exc) {
    unwind_state()->raise_exception(exc);
    return nullptr;
  }

  void ThreadState::raise_stack_error() {
    Class* stack_error = globals().stack_error.get();
    Exception* exc = memory()->new_object<Exception>(this, stack_error);
    exc->locations(this, Location::from_call_stack(this));
    unwind_state()->raise_exception(exc);
  }

  Object* ThreadState::park(STATE) {
    return vm_->park_->park(this);
  }

  Object* ThreadState::park_timed(STATE, struct timespec* ts) {
    return vm_->park_->park_timed(this, ts);
  }

  const uint32_t ThreadState::hash_seed() {
    return machine()->machine_state()->hash_seed();
  }

  MachineState* const ThreadState::machine_state() {
    return vm_->machine()->machine_state();
  }

  Machine* const ThreadState::machine() {
    return vm_->machine();
  }

  Configuration* const ThreadState::configuration() {
    return machine()->configuration();
  }

  Environment* const ThreadState::environment() {
    return machine()->environment();
  }

  ThreadNexus* const ThreadState::thread_nexus() {
    return machine()->thread_nexus();
  }

  Diagnostics* const ThreadState::diagnostics() {
    return machine()->diagnostics();
  }

  MachineThreads* const ThreadState::machine_threads() {
    return machine()->machine_threads();
  }

  memory::Collector* const ThreadState::collector() {
    return machine()->collector();
  }

  SignalThread* const ThreadState::signals() {
    return machine()->signals();
  }

  Memory* const ThreadState::memory() {
    return machine()->memory();
  }

  C_API* const ThreadState::c_api() {
    return machine()->c_api();
  }

  Profiler* const ThreadState::profiler() {
    return machine()->profiler();
  }

  Console* const ThreadState::console() {
    return machine()->console();
  }

  Globals& ThreadState::globals() {
    return memory()->globals;
  }

  Symbol* const ThreadState::symbol(const char* str) {
    return memory()->symbols.lookup(this, str, strlen(str));
  }

  Symbol* const ThreadState::symbol(const char* str, size_t len) {
    return memory()->symbols.lookup(this, str, len);
  }

  Symbol* const ThreadState::symbol(std::string str) {
    return memory()->symbols.lookup(this, str);
  }

  Symbol* const ThreadState::symbol(String* str) {
    return memory()->symbols.lookup(this, str);
  }
}
