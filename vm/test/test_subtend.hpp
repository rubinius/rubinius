#include <cxxtest/TestSuite.h>

/* System */
#include <ucontext.h>

/* Project */
#include "vm.hpp"
#include "message.hpp"
#include "builtin/task.hpp"

/* Testee */
#include "vmnativemethod.hpp"

static NativeMethodContext* hidden_context = NULL;

extern "C" {
  void dummy_function()
  {
    hidden_context = NativeMethodContext::current();
    hidden_context->action = NativeMethodContext::RETURN_FROM_C;
    return;
  }
}

using namespace rubinius;

class TestSubtend : public CxxTest::TestSuite
{
public:

  VM* my_state;

  void setUp() {
    my_state = new VM(1024);
  }

  void tearDown() {
    delete my_state;
  }

  /* Tests */

  void test_ruby_to_c()
  {
    Task* task = Task::create(my_state);
    Message message(my_state);

    bool ret = VMNativeMethod::executor(my_state, &dummy_function, task, &message);

    std::cout << ret << std::endl;
    TS_ASSERT(true == ret);
    TS_ASSERT(NULL != hidden_context);
  }
};
