/* Std */
#include <iostream>

/* Project */
#include "vmnativemethod.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/fixnum.hpp"

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
   *  @see  vm/builtin/nativemethod.hpp for the possible function signatures.
   */
  void VMNativeMethod::perform_call()
  {
    NativeMethodContext* context = NativeMethodContext::current();
    NativeMethod::PointerTo c_method = context->method->actual_function_object();

    HandleTo arg(context->handles, Fixnum::from(5));
    context->return_value = c_method(arg);

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
