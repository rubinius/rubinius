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
#include "builtin/nativemethodcontext.hpp"


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

    Object* first = as<Array>(ruby_array)->get(hidden_context->state(), 0);

    return HandleTo(hidden_context->handles(), first);
  }

  HandleTo minus_two_arity(HandleTo receiver, HandleTo ruby_array)
  {
    hidden_context = NativeMethodContext::current();
    hidden_receiver = as<Object>(receiver);
    hidden_ruby_array = as<Array>(ruby_array);

    Object* first = as<Array>(ruby_array)->get(hidden_context->state(), 0);

    return HandleTo(hidden_context->handles(), first);
  }

  HandleTo minus_one_arity(int argc, HandleTo* args, HandleTo receiver)
  {
    hidden_context = NativeMethodContext::current();
    hidden_argc = argc;
    hidden_c_array = args;
    hidden_receiver = as<Object>(receiver);

    return args[0];
  }

  HandleTo one_arg(HandleTo receiver, HandleTo arg1)
  {
    hidden_context = NativeMethodContext::current();
    hidden_receiver = as<Object>(receiver);

    return HandleTo(hidden_context->handles(), Fixnum::from(1));
  }

  HandleTo two_arg(HandleTo receiver, HandleTo arg1, HandleTo arg2)
  {
    hidden_context = NativeMethodContext::current();
    hidden_receiver = as<Object>(receiver);

    return HandleTo(hidden_context->handles(), Fixnum::from(2));
  }

  HandleTo three_arg(HandleTo receiver, HandleTo arg1, HandleTo arg2, HandleTo arg3)
  {
    hidden_context = NativeMethodContext::current();
    hidden_receiver = as<Object>(receiver);

    return HandleTo(hidden_context->handles(), Fixnum::from(3));
  }

  HandleTo four_arg(HandleTo receiver, HandleTo arg1, HandleTo arg2, HandleTo arg3, HandleTo arg4)
  {
    hidden_context = NativeMethodContext::current();
    hidden_receiver = as<Object>(receiver);

    return HandleTo(hidden_context->handles(), Fixnum::from(4));
  }

  HandleTo five_arg(HandleTo receiver, HandleTo arg1, HandleTo arg2, HandleTo arg3, HandleTo arg4, HandleTo arg5)
  {
    hidden_context = NativeMethodContext::current();
    hidden_receiver = as<Object>(receiver);

    return HandleTo(hidden_context->handles(), Fixnum::from(5));
  }
}


class TestSubtend : public CxxTest::TestSuite
{
  public:

  VM* my_state;
  Task* my_task;
  Message* my_message;

  void setUp() {
    my_state = new VM(65536 * 1000);
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
    my_message->name = my_state->symbol("__subtend_fake_test_method__");

    NativeMethod* method = NativeMethod::create(&minus_three_arity, ARGS_IN_RUBY_ARRAY);

    bool ret = method->execute(my_state, my_task, *my_message);

    TS_ASSERT_EQUALS(ret, true);

    TS_ASSERT_DIFFERS(hidden_context, static_cast<NativeMethodContext*>(NULL));

    TS_ASSERT_EQUALS(hidden_ruby_array, args);

    for (std::size_t i = 0; i < 10; ++i) {
      TS_ASSERT_EQUALS(hidden_ruby_array->get(my_state, i), control->get(my_state, i));
    }

    TS_ASSERT_EQUALS(hidden_context->return_value(), hidden_ruby_array->get(my_state, 0));
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
    my_message->name = my_state->symbol("__subtend_fake_test_method__");

    NativeMethod* method = NativeMethod::create(&minus_two_arity, RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY);

    bool ret = method->execute(my_state, my_task, *my_message);

    TS_ASSERT_EQUALS(ret, true);

    TS_ASSERT_DIFFERS(hidden_context, static_cast<NativeMethodContext*>(NULL));

    TS_ASSERT_EQUALS(hidden_receiver, receiver);
    TS_ASSERT_EQUALS(hidden_ruby_array, args);

    for (std::size_t i = 0; i < 10; ++i) {
      TS_ASSERT_EQUALS(hidden_ruby_array->get(my_state, i), control->get(my_state, i));
    }

    TS_ASSERT_EQUALS(hidden_context->return_value(), hidden_ruby_array->get(my_state, 0));
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
    my_message->name = my_state->symbol("__subtend_fake_test_method__");

    NativeMethod* method = NativeMethod::create(&minus_one_arity, ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER);

    bool ret = method->execute(my_state, my_task, *my_message);

    TS_ASSERT_EQUALS(ret, true);

    TS_ASSERT_DIFFERS(hidden_context, static_cast<NativeMethodContext*>(NULL));

    TS_ASSERT_EQUALS(hidden_receiver, receiver);
    TS_ASSERT_EQUALS(hidden_argc, 10);

    for (std::size_t i = 0; i < 10; ++i) {
      TS_ASSERT_EQUALS(as<Object>(hidden_c_array[i]), control->get(my_state, i));
    }

    TS_ASSERT_EQUALS(hidden_context->return_value(), control->get(my_state, 0));
  }

  /* TODO: Should check object identities too? */
  void test_ruby_to_c_call_with_recv_plus_one()
  {
    int arg_count = 1;

    Array* args = Array::create(my_state, arg_count);
    Array* control = Array::create(my_state, arg_count);

    for (int i = 0; i < arg_count; ++i) {
      Object* obj = my_state->new_object(my_state->globals.object.get());

      args->set(my_state, i, obj);
      control->set(my_state, i, obj);
    }

    Object* receiver = my_state->new_object(my_state->globals.object.get());

    my_message->recv = receiver;
    my_message->arguments = args;
    my_message->total_args = arg_count;
    my_message->name = my_state->symbol("__subtend_fake_test_method__");

    NativeMethod* method = NativeMethod::create(&one_arg, arg_count);

    method->execute(my_state, my_task, *my_message);

    TS_ASSERT_EQUALS(hidden_receiver, receiver);
    TS_ASSERT_EQUALS(as<Fixnum>(hidden_context->return_value())->to_int(), arg_count);
  }

  void test_ruby_to_c_call_with_recv_plus_two()
  {
    int arg_count = 2;

    Array* args = Array::create(my_state, arg_count);
    Array* control = Array::create(my_state, arg_count);

    for (int i = 0; i < arg_count; ++i) {
      Object* obj = my_state->new_object(my_state->globals.object.get());

      args->set(my_state, i, obj);
      control->set(my_state, i, obj);
    }

    Object* receiver = my_state->new_object(my_state->globals.object.get());

    my_message->recv = receiver;
    my_message->arguments = args;
    my_message->total_args = arg_count;
    my_message->name = my_state->symbol("__subtend_fake_test_method__");

    NativeMethod* method = NativeMethod::create(&two_arg, arg_count);

    method->execute(my_state, my_task, *my_message);

    TS_ASSERT_EQUALS(hidden_receiver, receiver);
    TS_ASSERT_EQUALS(as<Fixnum>(hidden_context->return_value())->to_int(), arg_count);
  }

  void test_ruby_to_c_call_with_recv_plus_three()
  {
    int arg_count = 3;

    Array* args = Array::create(my_state, arg_count);
    Array* control = Array::create(my_state, arg_count);

    for (int i = 0; i < arg_count; ++i) {
      Object* obj = my_state->new_object(my_state->globals.object.get());

      args->set(my_state, i, obj);
      control->set(my_state, i, obj);
    }

    Object* receiver = my_state->new_object(my_state->globals.object.get());

    my_message->recv = receiver;
    my_message->arguments = args;
    my_message->total_args = arg_count;
    my_message->name = my_state->symbol("__subtend_fake_test_method__");

    NativeMethod* method = NativeMethod::create(&three_arg, arg_count);

    method->execute(my_state, my_task, *my_message);

    TS_ASSERT_EQUALS(hidden_receiver, receiver);
    TS_ASSERT_EQUALS(as<Fixnum>(hidden_context->return_value())->to_int(), arg_count);
  }

  void test_ruby_to_c_call_with_recv_plus_four()
  {
    int arg_count = 4;

    Array* args = Array::create(my_state, arg_count);
    Array* control = Array::create(my_state, arg_count);

    for (int i = 0; i < arg_count; ++i) {
      Object* obj = my_state->new_object(my_state->globals.object.get());

      args->set(my_state, i, obj);
      control->set(my_state, i, obj);
    }

    Object* receiver = my_state->new_object(my_state->globals.object.get());

    my_message->recv = receiver;
    my_message->arguments = args;
    my_message->total_args = arg_count;
    my_message->name = my_state->symbol("__subtend_fake_test_method__");

    NativeMethod* method = NativeMethod::create(&four_arg, arg_count);

    method->execute(my_state, my_task, *my_message);

    TS_ASSERT_EQUALS(hidden_receiver, receiver);
    TS_ASSERT_EQUALS(as<Fixnum>(hidden_context->return_value())->to_int(), arg_count);
  }

  void test_ruby_to_c_call_with_recv_plus_five()
  {
    int arg_count = 5;

    Array* args = Array::create(my_state, arg_count);
    Array* control = Array::create(my_state, arg_count);

    for (int i = 0; i < arg_count; ++i) {
      Object* obj = my_state->new_object(my_state->globals.object.get());

      args->set(my_state, i, obj);
      control->set(my_state, i, obj);
    }

    Object* receiver = my_state->new_object(my_state->globals.object.get());

    my_message->recv = receiver;
    my_message->arguments = args;
    my_message->total_args = arg_count;
    my_message->name = my_state->symbol("__subtend_fake_test_method__");

    NativeMethod* method = NativeMethod::create(&five_arg, arg_count);

    method->execute(my_state, my_task, *my_message);

    TS_ASSERT_EQUALS(hidden_receiver, receiver);
    TS_ASSERT_EQUALS(as<Fixnum>(hidden_context->return_value())->to_int(), arg_count);
  }

};
