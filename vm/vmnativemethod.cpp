/* Std */
#include <iostream>

/* Project */
#include "builtin/array.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/nativemethodcontext.hpp"
#include "builtin/task.hpp"

#include "vmnativemethod.hpp"


namespace rubinius {


  /**
   *  This method always executes on the separate stack created for the context.
   *
   *  Fortunately for us, Message always has an Array of arguments.
   *
   *    Arity -3:   VALUE func(VALUE argument_array);
   *    Arity -2:   VALUE func(VALUE receiver, VALUE argument_array);
   *    Arity -1:   VALUE func(int argument_count, VALUE*, VALUE receiver);
   *    Otherwise:  VALUE func(VALUE receiver, VALUE arg1[, VALUE arg2, ...]);
   *
   *  @note   Currently supports functions with up to receiver + 5 (separate) arguments only!
   *          Anything beyond that should use one of the special arities instead.
   *
   *  TODO:   Argument count check?
   *  TODO:   Check for inefficiencies.
   */
  void VMNativeMethod::perform_call()
  {
    NativeMethodContext* context = NativeMethodContext::current();

    Message* message = context->message();

    HandleTo receiver(context->handles(), message->recv);

    switch (context->method()->arity()->to_int()) {
      case ARGS_IN_RUBY_ARRAY: {  /* Braces required to create objects in a switch */

        HandleTo args(context->handles(), message->arguments);
        context->return_value(as<Object>(context->method()->functor_as<OneArgFunctor>()(args)));

        break;
      }

      case RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY: {

        HandleTo args(context->handles(), message->arguments);
        context->return_value(as<Object>(context->method()->functor_as<TwoArgFunctor>()(receiver, args)));

        break;
      }

      case ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER: {

        HandleTo* args = new HandleTo[message->total_args];

        for (std::size_t i = 0; i < message->total_args; ++i) {
          args[i] = HandleTo(context->handles(), message->arguments->get(context->state(), i));
        }

        context->return_value(as<Object>(context->method()->functor_as<ArgcFunctor>()(message->total_args, args, receiver)));

        break;
      }

      /*
       *  Normal arg counts
       *
       *  Yes, it is ugly as fuck. It is intended as an encouragement
       *  to get rid of the concept of a separate Handle and Object.
       */

      case 0: {
        OneArgFunctor functor = context->method()->functor_as<OneArgFunctor>();
        context->return_value(as<Object>(functor(receiver)));
        break;
      }

      case 1: {
        TwoArgFunctor functor = context->method()->functor_as<TwoArgFunctor>();
        context->return_value(as<Object>(functor(receiver, Handle(context->handles(), message->arguments->get(context->state(), 0)) ) ));
        break;
      }

      case 2: {
        ThreeArgFunctor functor = context->method()->functor_as<ThreeArgFunctor>();
        context->return_value(as<Object>(functor( receiver
                                                , Handle(context->handles(), message->arguments->get(context->state(), 0)), Handle(context->handles(), message->arguments->get(context->state(), 1)) ) )
                             );
        break;
      }

      case 3: {
        FourArgFunctor functor = context->method()->functor_as<FourArgFunctor>();
        context->return_value(as<Object>(functor( receiver
                                                , Handle(context->handles(), message->arguments->get(context->state(), 0)), Handle(context->handles(), message->arguments->get(context->state(), 1))
                                                , Handle(context->handles(), message->arguments->get(context->state(), 2)) ) )
                             );
        break;
      }

      case 4: {
        FiveArgFunctor functor = context->method()->functor_as<FiveArgFunctor>();
        context->return_value(as<Object>(functor( receiver
                                                , Handle(context->handles(), message->arguments->get(context->state(), 0)), Handle(context->handles(), message->arguments->get(context->state(), 1))
                                                , Handle(context->handles(), message->arguments->get(context->state(), 2)), Handle(context->handles(), message->arguments->get(context->state(), 3)) ) )
                             );
        break;
      }

      case 5: {
        SixArgFunctor functor = context->method()->functor_as<SixArgFunctor>();
        context->return_value(as<Object>(functor( receiver
                                                , Handle(context->handles(), message->arguments->get(context->state(), 0)), Handle(context->handles(), message->arguments->get(context->state(), 1))
                                                , Handle(context->handles(), message->arguments->get(context->state(), 2)), Handle(context->handles(), message->arguments->get(context->state(), 3))
                                                , Handle(context->handles(), message->arguments->get(context->state(), 4)) ) )
                             );
        break;
      }

      default:
        sassert(false && "Not a valid arity");
    }

    context->action(NativeMethodContext::RETURN_FROM_C);

    jump_to_execution_point_in(context->dispatch_point());
    /* Never actually returns, control never reaches here. */
  }

  /**
   *  Execute the call to the C method.
   *
   *  Handles the returns, further calls, exceptions etc. too.
   *  The C methods always run in a separate machine-level
   *  stack.
   */
  bool VMNativeMethod::execute(VM* state, Task* task, Message* message, NativeMethod* method)
  {
    NativeMethodContext* context = NativeMethodContext::create(state, message, task, method);

    store_current_execution_point_in(context->dispatch_point());
    /* This is where control returns from calls */

    if (  NativeMethodContext::ORIGINAL_CALL == context->action()  ) {
      /* Actual dispatch must run in the new stack */
      create_execution_point_with_stack(context->c_call_point(), context->stack(), context->stacksize());
      set_function_to_run_in(context->c_call_point(), VMNativeMethod::perform_call);

      jump_to_execution_point_in(context->c_call_point());
    }

    return true;
  }
}
