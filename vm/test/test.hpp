#ifndef RBX_TEST_TEST_HPP
#define RBX_TEST_TEST_HPP

#include "vm.hpp"
#include "config_parser.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class VMTest {
public:
  SharedState* shared;
  VM* state;
  ConfigParser* config;

  void create() {
    shared = new SharedState();
    config = new ConfigParser;
    shared->user_config = config;
    state = shared->new_vm();
    state->initialize(VM::default_bytes);
    state->boot();

    state->global_lock().lock();
  }

  void destroy() {
    state->global_lock().unlock();
    VM::discard(state);
    SharedState::discard(shared);
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
