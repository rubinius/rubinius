#include <iostream>

#include "vm.hpp"

#include "native_libraries.hpp"
#include "primitives.hpp"
#include "quantum_stack_leap.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/string.hpp"
#include "builtin/task.hpp"
#include "builtin/tuple.hpp"

#include "message.hpp"

namespace rubinius {

  class Task;

/* Class methods */


  void NativeMethod::register_class_with(STATE) {
    state->globals.nmethod.set(state->new_class("NativeMethod", state->globals.executable.get(), NativeMethod::fields));
    state->globals.nmethod.get()->set_object_type(state, NativeMethodType);
  }

  NativeMethod* NativeMethod::allocate(STATE) {
    return create<GenericFunctor>(state);
  }

  /** @todo Set up a SIGSEGV/SIGBUS handler. */
  ExecuteStatus NativeMethod::activate_from(NativeMethodContext* context) {
    NativeMethodContext::current_context_is(context);

    store_current_execution_point_in(context->dispatch_point());

    /* This is where control returns from jumps. Regrab context. */
    context = NativeMethodContext::current();

    if(NativeMethodContext::ORIGINAL_CALL == context->action()) {
      /* Actual dispatch must run in the new stack */
      create_execution_point_with_stack(context->c_call_point(), context->stack(), context->stacksize());
      set_function_to_run_in(context->c_call_point(), NativeMethod::perform_call);

      jump_to_execution_point_in(context->c_call_point());
    }

    switch(context->action()) {
    case NativeMethodContext::CALL_FROM_C:

      /*  CompiledMethods are only loaded, not executed, so a
       *  So, we return from here which then allows the CM to really
       *  execute.
       */
      return context->task()->send_message_slowly(context->message_from_c());

    case NativeMethodContext::RETURNED_BACK_TO_C:
      jump_to_execution_point_in(context->inside_c_method_point());
      break;  /* Never reached */

    case NativeMethodContext::RETURN_FROM_C:
      context->task()->native_return(context->return_value());
      NativeMethodContext::current_context_is(NULL);
      break;

    case NativeMethodContext::SEGFAULT_DETECTED:
      break;

    default:
      break;
    }

    return cExecuteRestart;
  }

  ExecuteStatus NativeMethod::executor_implementation(STATE, Task* task, Message& message) {
    NativeMethodContext* context = NativeMethodContext::create(state,
        &message, task, as<NativeMethod>(message.method));

    task->active(state, context);

    return activate_from(context);
  }

  NativeMethod* NativeMethod::load_extension_entry_point(STATE, String* path, String* name) {
    void* func = NativeLibrary::find_symbol(state, name, path);

    NativeMethod* m = NativeMethod::create(state,
                                           path,
                                           state->globals.rubinius.get(),
                                           name->to_sym(state),
                                           reinterpret_cast<GenericFunctor>(func),
                                           Fixnum::from(INIT_FUNCTION)
                                          );
    return m;
  }

  /**
   *  This method always executes on the separate stack created for the context.
   *
   *    Arity -3:   VALUE func(VALUE argument_array);
   *    Arity -2:   VALUE func(VALUE receiver, VALUE argument_array);
   *    Arity -1:   VALUE func(int argument_count, VALUE*, VALUE receiver);
   *    Otherwise:  VALUE func(VALUE receiver, VALUE arg1[, VALUE arg2, ...]);
   *
   *  There is also a special-case arity, INIT_FUNCTION, which corresponds
   *  to void (*)(void) and should never appear in user code.
   *
   *  @note   Currently supports functions with up to receiver + 5 (separate) arguments only!
   *          Anything beyond that should use one of the special arities instead.
   *
   *  @todo   Check for inefficiencies.
   */
  void NativeMethod::perform_call() {
    NativeMethodContext* context = NativeMethodContext::current();

    Message* message = context->message();

    Handle receiver = context->handle_for(message->recv);

    try {

      switch (context->method()->arity()->to_int()) {
      case ARGS_IN_RUBY_ARRAY: {  /* Braces required to create objects in a switch */
        Handle args = context->handle_for(message->as_array(context->state()));

        Handle ret_handle = context->method()->functor_as<OneArgFunctor>()(args);

        context->return_value(context->object_from(ret_handle));
        break;
      }

      case RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY: {
        Handle args = context->handle_for(message->as_array(context->state()));

        Handle ret_handle = context->method()->functor_as<TwoArgFunctor>()(receiver, args);

        context = NativeMethodContext::current();
        context->return_value(context->object_from(ret_handle));
        break;
      }

      case ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER: {

        Handle* args = new Handle[message->args()];

        for (std::size_t i = 0; i < message->args(); ++i) {
          args[i] = context->handle_for(message->get_argument(i));
        }

        Handle ret_handle = context->method()->functor_as<ArgcFunctor>()(message->args(), args, receiver);

        context = NativeMethodContext::current();
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

        context = NativeMethodContext::current();
        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 1: {
        TwoArgFunctor functor = context->method()->functor_as<TwoArgFunctor>();

        Handle ret_handle = functor(receiver,
                                    context->handle_for(message->get_argument(0)));

        context = NativeMethodContext::current();
        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 2: {
        ThreeArgFunctor functor = context->method()->functor_as<ThreeArgFunctor>();

        Handle ret_handle = functor(receiver,
                                    context->handle_for(message->get_argument(0)),
                                    context->handle_for(message->get_argument(1))
                                   );

        context = NativeMethodContext::current();
        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 3: {
        FourArgFunctor functor = context->method()->functor_as<FourArgFunctor>();

        Handle ret_handle = functor(receiver,
                                    context->handle_for(message->get_argument(0)),
                                    context->handle_for(message->get_argument(1)),
                                    context->handle_for(message->get_argument(2))
                                   );

        context = NativeMethodContext::current();
        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 4: {
        FiveArgFunctor functor = context->method()->functor_as<FiveArgFunctor>();

        Handle ret_handle = functor(receiver,
                                    context->handle_for(message->get_argument(0)),
                                    context->handle_for(message->get_argument(1)),
                                    context->handle_for(message->get_argument(2)),
                                    context->handle_for(message->get_argument(3))
                                   );

        context = NativeMethodContext::current();
        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 5: {
        SixArgFunctor functor = context->method()->functor_as<SixArgFunctor>();

        Handle ret_handle = functor(receiver,
                                    context->handle_for(message->get_argument(0)),
                                    context->handle_for(message->get_argument(1)),
                                    context->handle_for(message->get_argument(2)),
                                    context->handle_for(message->get_argument(3)),
                                    context->handle_for(message->get_argument(4))
                                   );

        context = NativeMethodContext::current();
        context->return_value(context->object_from(ret_handle));
        break;
      }

      /* Extension entry point, should never occur for user code. */
      case INIT_FUNCTION: {
        InitFunctor functor = context->method()->functor_as<InitFunctor>();

        functor();

        context = NativeMethodContext::current();
        context->return_value(Qnil);
        break;
      }

      default:
        sassert(false && "Not a valid arity");
      }

    }
    catch(const std::exception& ex) {
      std::cerr << "Error in perform_call(): " << ex.what() << std::endl;
      context->action(NativeMethodContext::ERROR_RAISED);
      goto leave;
    }
    catch(...) {
      std::cerr << "UNKNOWN error in perform_call()!" << std::endl;
      context->action(NativeMethodContext::ERROR_RAISED);
      goto leave;
    }

    context->action(NativeMethodContext::RETURN_FROM_C);

  leave:

    jump_to_execution_point_in(context->dispatch_point());
    /* Never actually returns, control never reaches here. */
  }

}
