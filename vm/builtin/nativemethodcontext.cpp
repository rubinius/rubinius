#include "builtin/nativemethodcontext.hpp"
#include "builtin/task.hpp"

namespace rubinius {

  /**
   *  Currently active NativeMethodContext access.
   *
   *  TODO: use thread-local.
   */
  static NativeMethodContext* hidden_current_native_context = NULL;


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

    /* TODO: Make active here. */

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

}

