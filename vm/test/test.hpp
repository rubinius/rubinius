#ifndef RBX_TEST_TEST_HPP
#define RBX_TEST_TEST_HPP

#include "vm.hpp"
#include "vm_manager.hpp"
#include "config_parser.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class VMTest {
public:
  VMManager* manager;
  SharedState* shared;
  VM* state;
  ConfigParser* config;

  void create() {
    manager = new VMManager;
    shared = manager->create_shared_state();
    config = new ConfigParser;
    shared->user_config = config;
    state = shared->new_vm();
    state->initialize(VM::default_bytes);
    state->boot();
  }

  void destroy() {
    state->global_lock().unlock();
    manager->destroy_vm(state);
    delete manager;
  }

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  VM* new_vm() {
    return shared->new_vm();
  }

};

#endif
