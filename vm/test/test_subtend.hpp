#include <cxxtest/TestSuite.h>

/* System */
#include <ucontext.h>

/* Project */
#include "vm.hpp"
#include "message.hpp"
#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/task.hpp"

/* Testee */
#include "vmnativemethod.hpp"
#include "builtin/nativemethod.hpp"


static NativeMethodContext* hidden_context = NULL;
static Array* hidden_ruby_array = NULL;
static Object* hidden_receiver = NULL;
static int hidden_argc = 0;
static HandleTo* hidden_c_array = NULL;

using namespace rubinius;

extern "C" {

  HandleTo minus_three_arity(HandleTo ruby_array)
  {
    hidden_context = NativeMethodContext::current();
    hidden_ruby_array = as<Array>(ruby_array);

    Object* first = as<Array>(ruby_array)->get(hidden_context->state, 0);

    return HandleTo(hidden_context->handles, first);
  }

  HandleTo minus_two_arity(HandleTo receiver, HandleTo ruby_array)
  {
    hidden_context = NativeMethodContext::current();
    hidden_receiver = as<Object>(receiver);
    hidden_ruby_array = as<Array>(ruby_array);

    Object* first = as<Array>(ruby_array)->get(hidden_context->state, 0);

    return HandleTo(hidden_context->handles, first);
  }

  HandleTo minus_one_arity(int argc, HandleTo* args, HandleTo receiver)
  {
    hidden_context = NativeMethodContext::current();
    hidden_argc = argc;
    hidden_c_array = args;
    hidden_receiver = as<Object>(receiver);

    return args[0];
  }

}


class TestSubtend : public CxxTest::TestSuite
{
  public:

  VM* my_state;
  Task* my_task;
  Message* my_message;

  void setUp() {
    my_state = new VM(2048);
    my_task = Task::create(my_state);
    my_message = new Message(my_state);
  }

  void tearDown() {
    hidden_context = NULL;
    hidden_ruby_array = NULL;
    hidden_argc = 0;
    hidden_c_array = NULL;

    delete my_message;
    delete my_state;
  }

  /* Tests */

  void test_ruby_to_c_call_with_args_as_ruby_array()
  {
    Array* args = Array::create(my_state, 10);
    Array* control = Array::create(my_state, 10);

    for (std::size_t i = 0; i < 10; ++i) {
      Object* obj = my_state->new_object(my_state->globals.object.get());

      args->set(my_state, i, obj);
      control->set(my_state, i, obj);
    }

    my_message->arguments = args;

    NativeMethod* method = NativeMethod::create(&minus_three_arity, ARGS_IN_RUBY_ARRAY);

    bool ret = method->execute(my_state, my_task, *my_message);

    TS_ASSERT_EQUALS(ret, true);

    TS_ASSERT_DIFFERS(hidden_context, static_cast<NativeMethodContext*>(NULL));

    TS_ASSERT_EQUALS(hidden_ruby_array, args);

    for (std::size_t i = 0; i < 10; ++i) {
      TS_ASSERT_EQUALS(hidden_ruby_array->get(my_state, i), control->get(my_state, i));
    }

    TS_ASSERT_EQUALS(hidden_context->return_value, hidden_ruby_array->get(my_state, 0));
  }

  void test_ruby_to_c_call_with_receiver_and_args_as_ruby_array()
  {
    Array* args = Array::create(my_state, 10);
    Array* control = Array::create(my_state, 10);

    for (std::size_t i = 0; i < 10; ++i) {
      Object* obj = my_state->new_object(my_state->globals.object.get());

      args->set(my_state, i, obj);
      control->set(my_state, i, obj);
    }

    Object* receiver = my_state->new_object(my_state->globals.object.get());

    my_message->recv = receiver;
    my_message->arguments = args;

    NativeMethod* method = NativeMethod::create(&minus_two_arity, RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY);

    bool ret = method->execute(my_state, my_task, *my_message);

    TS_ASSERT_EQUALS(ret, true);

    TS_ASSERT_DIFFERS(hidden_context, static_cast<NativeMethodContext*>(NULL));

    TS_ASSERT_EQUALS(hidden_receiver, receiver);
    TS_ASSERT_EQUALS(hidden_ruby_array, args);

    for (std::size_t i = 0; i < 10; ++i) {
      TS_ASSERT_EQUALS(hidden_ruby_array->get(my_state, i), control->get(my_state, i));
    }

    TS_ASSERT_EQUALS(hidden_context->return_value, hidden_ruby_array->get(my_state, 0));
  }

  void test_ruby_to_c_call_with_arg_count_args_in_c_array_plus_receiver()
  {
    Array* args = Array::create(my_state, 10);
    Array* control = Array::create(my_state, 10);

    for (std::size_t i = 0; i < 10; ++i) {
      Object* obj = my_state->new_object(my_state->globals.object.get());

      args->set(my_state, i, obj);
      control->set(my_state, i, obj);
    }

    Object* receiver = my_state->new_object(my_state->globals.object.get());

    my_message->recv = receiver;
    my_message->arguments = args;
    my_message->total_args = 10;

    NativeMethod* method = NativeMethod::create(&minus_one_arity, ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER);

    bool ret = method->execute(my_state, my_task, *my_message);

    TS_ASSERT_EQUALS(ret, true);

    TS_ASSERT_DIFFERS(hidden_context, static_cast<NativeMethodContext*>(NULL));

    TS_ASSERT_EQUALS(hidden_receiver, receiver);
    TS_ASSERT_EQUALS(hidden_argc, 10);

    for (std::size_t i = 0; i < 10; ++i) {
      TS_ASSERT_EQUALS(as<Object>(hidden_c_array[i]), control->get(my_state, i));
    }

    TS_ASSERT_EQUALS(hidden_context->return_value, control->get(my_state, 0));
  }

};
