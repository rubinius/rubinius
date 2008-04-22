#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;
class TestIso : public CxxTest::TestSuite {
  public:

#undef state

  void test_isolated() {
    VM* state = new VM();
    Task* task = Task::create(state);

    task->check_interrupts();
  }
};
