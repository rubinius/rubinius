#ifndef RBX_TEST_TEST_HPP
#define RBX_TEST_TEST_HPP

#include "vm.hpp"
#include "config_parser.hpp"
#include "vm/object_utils.hpp"
#include "object_memory.hpp"
#include "configuration.hpp"
#include "vm/detection.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class VMTest {
public:
  SharedState* shared;
  State* state;
  ConfigParser* config_parser;
  Configuration config;

  void create() {
    config_parser = new ConfigParser;
    shared = new SharedState(0, config, *config_parser);
    VM* vm = shared->new_vm();
    vm->initialize_as_root();
    state = new State(vm);
  }

  void destroy() {
    VM::discard(state, state->vm());
    SharedState::discard(shared);
    delete state;
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
