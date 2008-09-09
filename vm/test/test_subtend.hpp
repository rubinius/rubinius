#include <cxxtest/TestSuite.h>

/* System */
#include <ucontext.h>

/* Project */
#include "vm.hpp"
#include "message.hpp"
#include "builtin/task.hpp"

/* Testee */
#include "vmnativemethod.hpp"
#include "builtin/nativemethod.hpp"

static NativeMethodContext* hidden_context = NULL;
static int hidden_given_int = 0;

extern "C" {
  int dummy_function(int i)
  {
    hidden_context = NativeMethodContext::current();
    hidden_given_int = i;

    return i + i;
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
    hidden_context = NULL;
    hidden_given_int = 0;
    delete my_state;
  }

  /* Tests */

  void test_ruby_to_c_call()
  {
    Task* task = Task::create(my_state);
    Message message(my_state);

    NativeMethod* method = NativeMethod::create(reinterpret_cast<NativeMethod::PointerTo>(&dummy_function));

    bool ret = method->execute(my_state, task, message);

    TS_ASSERT(true == ret);

    TS_ASSERT(NULL != hidden_context);
    TS_ASSERT(0 != hidden_given_int);

    TS_ASSERT(5 == hidden_given_int);
    TS_ASSERT(10 == hidden_context->return_value);
  }
};
