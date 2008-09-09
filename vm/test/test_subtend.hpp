#include <cxxtest/TestSuite.h>

/* System */
#include <ucontext.h>

/* Project */
#include "vm.hpp"
#include "message.hpp"
#include "builtin/task.hpp"
#include "builtin/fixnum.hpp"

/* Testee */
#include "vmnativemethod.hpp"
#include "builtin/nativemethod.hpp"

static NativeMethodContext* hidden_context = NULL;
static int hidden_given_int = 0;

using namespace rubinius;

extern "C" {
  HandleTo dummy_function(HandleTo obj)
  {
    hidden_context = NativeMethodContext::current();
    hidden_given_int = as<Fixnum>(obj)->to_native();

    return Handle(hidden_context->handles, Fixnum::from(hidden_given_int * 2));
  }
}


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

    NativeMethod* method = NativeMethod::create(&dummy_function);

    bool ret = method->execute(my_state, task, message);

    TS_ASSERT(true == ret);

    TS_ASSERT(NULL != hidden_context);

    TS_ASSERT(5 == hidden_given_int);
    TS_ASSERT(10 == as<Fixnum>(hidden_context->return_value)->to_native());
  }
};
