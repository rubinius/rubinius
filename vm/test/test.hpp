#ifndef RBX_TEST_TEST_HPP
#define RBX_TEST_TEST_HPP

#include "vm.hpp"
#include "config_parser.hpp"
#include "vm/object_utils.hpp"
#include "object_memory.hpp"
#include "configuration.hpp"
#include "metrics.hpp"
#include "vm/detection.hpp"
#include "vm/gc/immix_marker.hpp"

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
    vm->metrics().init(metrics::eRubyMetrics);
    state = new State(vm);
  }

  void destroy() {
    if(ObjectMemory* om = state->memory()) {
      if(ImmixMarker* im = om->immix_marker()) {
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
