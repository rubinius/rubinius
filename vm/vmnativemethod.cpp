/* Std */
#include <iostream>

/* Project */
#include "vmnativemethod.hpp"

#include "builtin/array.hpp"
#include "builtin/nativemethod.hpp"

namespace rubinius {

  class NativeMethod;

  /** Currently active NativeMethodContext access.
   *
   *  TODO: use thread-local.
   */
  static NativeMethodContext* hidden_current_native_context = NULL;


/* NativeMethodContext */

    /**
     *  Brand new context for a brand new call.
     */
    NativeMethodContext* NativeMethodContext::create(Message* msg, Task* task, NativeMethod* method)
    {
      NativeMethodContext* nmc = new NativeMethodContext();

      nmc->action = ORIGINAL_CALL;

      nmc->stack_size = DEFAULT_STACK_SIZE;
      nmc->stack = new char[nmc->stack_size];

      nmc->message = msg;
      nmc->task = task;
      nmc->state = task->state;

      nmc->method = method;

      nmc->return_value = 0;

      /* HACK: Set contexts up so that Task::make_active() is transparent */
//      nmc->sender = task->active;
//      SET(task, literals, Qnil);
//      SET(task, active, nmc);
//      SET(task, home, nmc);
//      SET(task, self, message.current_self);

      current_context_is(nmc);

      return nmc;
    }

  /**
   *  Set the currently active NativeMethodContext.
   */
  void NativeMethodContext::current_context_is(NativeMethodContext* context)
  {
    hidden_current_native_context = context;
  }

  /**
   *  Access the currently active NativeMethodContext.
   *
   *  TODO:   Exception handling.
   *  TODO:   Use reference.
   */
  NativeMethodContext* NativeMethodContext::current()
  {
     return hidden_current_native_context;
  }



/* VMNativeMethod */


  /**
   *  This method always executes on the separate stack created for the context.
   *
   *  Fortunately for us, Message always has an Array of arguments.
   *
   *    Arity -3:   VALUE func(VALUE argument_array);
   *    Arity -2:   VALUE func(VALUE receiver, VALUE argument_array);
   *    Arity -1:   VALUE func(int argument_count, VALUE*, VALUE receiver);
   *    Otherwise:  VALUE func(VALUE receiver, VALUE arg1[, VALUE arg2, ...]);    // Currently max 10 args
   *
   *  TODO:   Argument count check?
   *  TODO:   Check for inefficiencies.
   */
  void VMNativeMethod::perform_call()
  {
    NativeMethodContext* context = NativeMethodContext::current();

    Message* message = context->message;

    HandleTo receiver(context->handles, message->recv);

    switch (context->method->arity()->to_int()) {
      case ARGS_IN_RUBY_ARRAY: {  /* Braces required to create objects in a switch */

        HandleTo args(context->handles, message->arguments);
        context->return_value = as<Object>(context->method->functor_as<OneArgFunctor>()(args));

        break;
      }

      case RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY: {

        HandleTo args(context->handles, message->arguments);
        context->return_value = as<Object>(context->method->functor_as<TwoArgFunctor>()(receiver, args));

        break;
      }

      case ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER: {

        HandleTo* args = new HandleTo[message->total_args];

        for (std::size_t i = 0; i < message->total_args; ++i) {
          args[i] = HandleTo(context->handles, message->arguments->get(context->state, i));
        }

        context->return_value = as<Object>(context->method->functor_as<ArgcFunctor>()(message->total_args, args, receiver));

        break;
      }

      default:
        sassert(false && "Not a valid arity, wth?");
    }

    context->action = NativeMethodContext::RETURN_FROM_C;

    jump_to_execution_point_in(context->dispatch_point);
    /* Never actually returns, control never reaches here. */
  }

  bool VMNativeMethod::execute(VM* state, Task* task, Message* message, NativeMethod* method)
  {
    NativeMethodContext* context = NativeMethodContext::create(message, task, method);

    store_current_execution_point_in(context->dispatch_point);
    /* This is where control returns from calls */

    if (  NativeMethodContext::ORIGINAL_CALL == context->action  ) {
      /* Actual dispatch must run in the new stack */
      create_execution_point_with_stack(context->c_call_point, context->stack, context->stack_size);
      set_function_to_run_in(context->c_call_point, VMNativeMethod::perform_call);

      jump_to_execution_point_in(context->c_call_point);
    }

    return true;
  }
}
