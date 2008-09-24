#include "vm.hpp"

#include "quantum_stack_leap.hpp"

#include "builtin/array.hpp"
#include "builtin/nativemethod.hpp"


namespace rubinius {

  class Task;
  class Message;


/* Class methods */


  void NativeMethod::register_class_with(VM* state)
  {
    state->globals.nmethod.set(state->new_class("NativeMethod", state->globals.executable.get(), NativeMethod::fields));
    state->globals.nmethod.get()->set_object_type(state, NativeMethodType);
  }

  NativeMethod* NativeMethod::allocate(VM* state)
  {
    return create<GenericFunctor>(state);
  }

  /**
   *  TODO: Context activation/reactivation needs to be redone.
   */
  bool NativeMethod::executor_implementation(VM* state, Executable* method, Task* task, Message& message)
  {
    NativeMethodContext* context = NativeMethodContext::create(state, &message, task, as<NativeMethod>(method));

    store_current_execution_point_in(context->dispatch_point());
    /* This is where control returns from calls */

    if (  NativeMethodContext::ORIGINAL_CALL == context->action()  ) {
      /* Actual dispatch must run in the new stack */
      create_execution_point_with_stack(context->c_call_point(), context->stack(), context->stacksize());
      set_function_to_run_in(context->c_call_point(), NativeMethod::perform_call);

      jump_to_execution_point_in(context->c_call_point());
    }

    switch (context->action()) {
      case NativeMethodContext::CALL_FROM_C:

        task->send_message_slowly(context->message_from_c());

        /* CompiledMethods are only loaded, not executed, so a
         * different active context here means the call is "pending."
         * So, we return from here which then allows the CM to really
         * execute. Execution returns here when we are reactivated.
         */
        if (task->active() != this) break;

        context->action(NativeMethodContext::RETURNED_BACK_TO_C);

        jump_to_execution_point_in(context->inside_c_method_point());
        break;
    }

    return true;
  }

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
  void NativeMethod::perform_call()
  {
    NativeMethodContext* context = NativeMethodContext::current();

    Message* message = context->message();

    Handle receiver = context->handle_for(message->recv);

    switch (context->method()->arity()->to_int()) {
      case ARGS_IN_RUBY_ARRAY: {  /* Braces required to create objects in a switch */
        Handle args = context->handle_for(message->arguments);

        Handle ret_handle = context->method()->functor_as<OneArgFunctor>()(args);

        context->return_value(context->object_from(ret_handle));
        break;
      }

      case RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY: {
        Handle args = context->handle_for(message->arguments);

        Handle ret_handle = context->method()->functor_as<TwoArgFunctor>()(receiver, args);

        context->return_value(context->object_from(ret_handle));
        break;
      }

      case ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER: {

        Handle* args = new Handle[message->total_args];

        for (std::size_t i = 0; i < message->total_args; ++i) {
          args[i] = context->handle_for(message->arguments->get(context->state(), i));
        }

        Handle ret_handle = context->method()->functor_as<ArgcFunctor>()(message->total_args, args, receiver);

        context->return_value(context->object_from(ret_handle));
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

        Handle ret_handle = functor(receiver);

        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 1: {
        TwoArgFunctor functor = context->method()->functor_as<TwoArgFunctor>();

        Handle ret_handle = functor(receiver,
                                    context->handle_for(message->arguments->get(context->state(), 0)));

        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 2: {
        ThreeArgFunctor functor = context->method()->functor_as<ThreeArgFunctor>();

        Handle ret_handle = functor(receiver,
                                    context->handle_for(message->arguments->get(context->state(), 0)),
                                    context->handle_for(message->arguments->get(context->state(), 1))
                                   );

        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 3: {
        FourArgFunctor functor = context->method()->functor_as<FourArgFunctor>();

        Handle ret_handle = functor(receiver,
                                    context->handle_for(message->arguments->get(context->state(), 0)),
                                    context->handle_for(message->arguments->get(context->state(), 1)),
                                    context->handle_for(message->arguments->get(context->state(), 2))
                                   );

        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 4: {
        FiveArgFunctor functor = context->method()->functor_as<FiveArgFunctor>();

        Handle ret_handle = functor(receiver,
                                    context->handle_for(message->arguments->get(context->state(), 0)),
                                    context->handle_for(message->arguments->get(context->state(), 1)),
                                    context->handle_for(message->arguments->get(context->state(), 2)),
                                    context->handle_for(message->arguments->get(context->state(), 3))
                                   );

        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 5: {
        SixArgFunctor functor = context->method()->functor_as<SixArgFunctor>();

        Handle ret_handle = functor(receiver,
                                    context->handle_for(message->arguments->get(context->state(), 0)),
                                    context->handle_for(message->arguments->get(context->state(), 1)),
                                    context->handle_for(message->arguments->get(context->state(), 2)),
                                    context->handle_for(message->arguments->get(context->state(), 3)),
                                    context->handle_for(message->arguments->get(context->state(), 4))
                                   );

        context->return_value(context->object_from(ret_handle));
        break;
      }

      default:
        sassert(false && "Not a valid arity");
    }

    context->action(NativeMethodContext::RETURN_FROM_C);

    jump_to_execution_point_in(context->dispatch_point());
    /* Never actually returns, control never reaches here. */
  }


}
