#ifndef RBX_TEST_TEST_HPP
#define RBX_TEST_TEST_HPP

#include "vm.hpp"
#include "config_parser.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "configuration.hpp"
#include "vm/detection.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class VMTest {
public:
  SharedState* shared;
  VM* state;
  ConfigParser* config_parser;
  Configuration config;

  void create() {
    config_parser = new ConfigParser;
    shared = new SharedState(0, config, *config_parser);
    state = shared->new_vm();
    state->initialize();
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
