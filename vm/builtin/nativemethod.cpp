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

  typedef TypedRoot<Object*> RootHandle;

/* Class methods */

  thread::ThreadData<NativeMethodFraming*> native_method_framing;
  NativeMethodFraming* NativeMethodFraming::get() {
    return native_method_framing.get();
  }

  Handle NativeMethodFrame::get_handle(STATE, Object* obj) {
    Handle hndl = handles_.size();
    handles_.push_back(new RootHandle(state, obj));
    return hndl;
  }

  Object* NativeMethodFrame::get_object(Handle hndl) {
    return handles_[hndl]->get();
  }

  Handle NativeMethodFraming::get_handle(Object* obj) {
    return current_nmc_frame_->get_handle(state_, obj);
  }

  Handle NativeMethodFraming::get_handle_global(Object* obj) {
    Handle hndl = -1 - global_handles_.size();
    global_handles_.push_back(new RootHandle(state_, obj));
    return hndl;
  }

  Object* NativeMethodFraming::get_object(Handle hndl) {
    if(hndl < 0) {
      return global_handles_[-1 - hndl]->get();
    } else {
      return current_nmc_frame_->get_object(hndl);
    }
  }

  void NativeMethodFraming::delete_global(Handle hndl) {
    RootHandle* root = global_handles_[-1 - hndl];
    delete root;
    global_handles_[-1 - hndl] = 0;
  }

  Object* NativeMethodFraming::block() {
    return current_call_frame_->top_scope->block();
  }

  Handles& NativeMethodFraming::handles() {
    return current_nmc_frame_->handles();
  }

  void NativeMethod::register_class_with(STATE) {
    state->globals.nmethod.set(state->new_class("NativeMethod", G(executable)));
    state->globals.nmethod.get()->set_object_type(state, NativeMethodType);

    native_method_framing.set(new NativeMethodFraming);
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
      context->task()->send_message_slowly(NULL, context->message_from_c());
      return cExecuteRestart;

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

  Object* NativeMethod::executor_implementation(STATE, CallFrame* call_frame, Message& msg) {
    NativeMethodFraming* framing = native_method_framing.get();
    NativeMethodFrame nmf(framing->current_nmc_frame());

    framing->set_current_call_frame(call_frame);
    framing->set_current_nmc_frame(&nmf);
    framing->set_state(state);

    NativeMethod* nm = as<NativeMethod>(msg.method);
    Object* ret = nm->call(state, &nmf, msg);

    framing->set_current_nmc_frame(nmf.previous());

    return ret;
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

  Object* NativeMethod::call(STATE, NativeMethodFrame* frame, Message& msg) {
    Handle receiver = frame->get_handle(state, msg.recv);

    switch(arity()->to_int()) {
    case ARGS_IN_RUBY_ARRAY: {  /* Braces required to create objects in a switch */
      Handle args = frame->get_handle(state, msg.as_array(state));

      Handle ret_handle = functor_as<OneArgFunctor>()(args);

      return frame->get_object(ret_handle);
    }

    case RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY: {
      Handle args = frame->get_handle(state, msg.as_array(state));

      Handle ret_handle = functor_as<TwoArgFunctor>()(receiver, args);

      return frame->get_object(ret_handle);
    }

    case ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER: {
      Handle* args = new Handle[msg.args()];

      for (std::size_t i = 0; i < msg.args(); ++i) {
        args[i] = frame->get_handle(state, msg.get_argument(i));
      }

      Handle ret_handle = functor_as<ArgcFunctor>()(msg.args(), args, receiver);

      delete args;

      return frame->get_object(ret_handle);
    }

      /*
       *  Normal arg counts
       *
       *  Yes, it is ugly as fuck. It is intended as an encouragement
       *  to get rid of the concept of a separate Handle and Object.
       */

    case 0: {
      OneArgFunctor functor = functor_as<OneArgFunctor>();

      Handle ret_handle = functor(receiver);

      return frame->get_object(ret_handle);
    }

    case 1: {
      TwoArgFunctor functor = functor_as<TwoArgFunctor>();

      Handle a1 = frame->get_handle(state, msg.get_argument(0));

      Handle ret_handle = functor(receiver, a1);

      return frame->get_object(ret_handle);
    }

    case 2: {
      ThreeArgFunctor functor = functor_as<ThreeArgFunctor>();

      Handle a1 = frame->get_handle(state, msg.get_argument(0));
      Handle a2 = frame->get_handle(state, msg.get_argument(1));

      Handle ret_handle = functor(receiver, a1, a2);

      return frame->get_object(ret_handle);
    }

    case 3: {
      FourArgFunctor functor = functor_as<FourArgFunctor>();
      Handle a1 = frame->get_handle(state, msg.get_argument(0));
      Handle a2 = frame->get_handle(state, msg.get_argument(1));
      Handle a3 = frame->get_handle(state, msg.get_argument(2));

      Handle ret_handle = functor(receiver, a1, a2, a3);

      return frame->get_object(ret_handle);
    }

    case 4: {
      FiveArgFunctor functor = functor_as<FiveArgFunctor>();
      Handle a1 = frame->get_handle(state, msg.get_argument(0));
      Handle a2 = frame->get_handle(state, msg.get_argument(1));
      Handle a3 = frame->get_handle(state, msg.get_argument(2));
      Handle a4 = frame->get_handle(state, msg.get_argument(3));

      Handle ret_handle = functor(receiver, a1, a2, a3, a4);

      return frame->get_object(ret_handle);
    }

    case 5: {
      SixArgFunctor functor = functor_as<SixArgFunctor>();
      Handle a1 = frame->get_handle(state, msg.get_argument(0));
      Handle a2 = frame->get_handle(state, msg.get_argument(1));
      Handle a3 = frame->get_handle(state, msg.get_argument(2));
      Handle a4 = frame->get_handle(state, msg.get_argument(3));
      Handle a5 = frame->get_handle(state, msg.get_argument(4));

      Handle ret_handle = functor(receiver, a1, a2, a3, a4, a5);

      return frame->get_object(ret_handle);
    }

      /* Extension entry point, should never occur for user code. */
    case INIT_FUNCTION: {
      InitFunctor functor = functor_as<InitFunctor>();

      functor();

      return Qnil;
    }

    default:
      assert(false && "Not a valid arity");
      return Qnil;
    }

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

        Handle a1 = context->handle_for(message->get_argument(0));

        Handle ret_handle = functor(receiver, a1);

        context = NativeMethodContext::current();
        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 2: {
        ThreeArgFunctor functor = context->method()->functor_as<ThreeArgFunctor>();

        Handle a1 = context->handle_for(message->get_argument(0));
        Handle a2 = context->handle_for(message->get_argument(1));

        Handle ret_handle = functor(receiver, a1, a2);

        context = NativeMethodContext::current();
        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 3: {
        FourArgFunctor functor = context->method()->functor_as<FourArgFunctor>();
        Handle a1 = context->handle_for(message->get_argument(0));
        Handle a2 = context->handle_for(message->get_argument(1));
        Handle a3 = context->handle_for(message->get_argument(2));

        Handle ret_handle = functor(receiver, a1, a2, a3);

        context = NativeMethodContext::current();
        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 4: {
        FiveArgFunctor functor = context->method()->functor_as<FiveArgFunctor>();
        Handle a1 = context->handle_for(message->get_argument(0));
        Handle a2 = context->handle_for(message->get_argument(1));
        Handle a3 = context->handle_for(message->get_argument(2));
        Handle a4 = context->handle_for(message->get_argument(3));

        Handle ret_handle = functor(receiver, a1, a2, a3, a4);

        context = NativeMethodContext::current();
        context->return_value(context->object_from(ret_handle));
        break;
      }

      case 5: {
        SixArgFunctor functor = context->method()->functor_as<SixArgFunctor>();
        Handle a1 = context->handle_for(message->get_argument(0));
        Handle a2 = context->handle_for(message->get_argument(1));
        Handle a3 = context->handle_for(message->get_argument(2));
        Handle a4 = context->handle_for(message->get_argument(3));
        Handle a5 = context->handle_for(message->get_argument(4));

        Handle ret_handle = functor(receiver, a1, a2, a3, a4, a5);

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
