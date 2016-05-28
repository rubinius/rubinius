#ifndef RBX_TEST_TEST_HPP
#define RBX_TEST_TEST_HPP

#include "vm.hpp"
#include "state.hpp"
#include "config_parser.hpp"
#include "machine/object_utils.hpp"
#include "memory.hpp"
#include "configuration.hpp"
#include "metrics.hpp"
#include "machine/detection.hpp"
#include "machine/memory/immix_marker.hpp"
#include "builtin/thread.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class VMTest {
public:
  SharedState* shared;
  State* state;
  ConfigParser* config_parser;
  Configuration config;

  // TODO: Fix this
  void initialize_as_root(VM* vm) {
    vm->set_current_thread();

    Memory* om = new Memory(vm, vm->shared);
    vm->shared.om = om;

    vm->shared.set_initialized();
    vm->shared.set_root_vm(vm);

    vm->managed_phase();

    State state(vm);

    TypeInfo::auto_learn_fields(&state);

    vm->bootstrap_ontology(&state);

    // Setup the main Thread, which is wrapper of the main native thread
    // when the VM boots.
    Thread::create(&state, vm);
    vm->thread->alive(&state, cTrue);
    vm->thread->sleep(&state, cFalse);
  }

  void create() {
    config_parser = new ConfigParser;
    shared = new SharedState(0, config, *config_parser);
    VM* vm = shared->thread_nexus()->new_vm(shared);
    initialize_as_root(vm);
    state = new State(vm);
  }

  void destroy() {
    if(Memory* om = state->memory()) {
      if(memory::ImmixMarker* im = om->immix_marker()) {
        im->stop(state);
      }
    }

    VM::discard(state, state->vm());
    delete shared;
    delete state;
  }

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }
};

#endif
