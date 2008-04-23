#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;
class TestIso : public CxxTest::TestSuite {
  public:

#undef state

  void test_gc_cycle() {
    VM* state = new VM();
    state->collect();
    state->collect();
    state->collect();
    state->collect();
  }
};
