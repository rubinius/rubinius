#ifndef RBX_VMNATIVEMETHOD_HPP
#define RBX_VMNATIVEMETHOD_HPP

/* Std */
#include <vector>

/* System */
#include <ucontext.h>

/* Project */
#include "vmexecutable.hpp"


/* Some alterable labels to create an API. Sadly #defines are a must here. */
#define store_current_execution_point_in(uctx)      getcontext((uctx))
#define jump_to_execution_point_in(uctx)            setcontext((uctx))

#define create_execution_point_with_stack(uctx, stack, size) do { getcontext((uctx)); \
                                                                  (uctx)->uc_link = NULL; \
                                                                  (uctx)->uc_stack.ss_sp = (stack); \
                                                                  (uctx)->uc_stack.ss_size = (size); \
                                                                  (uctx)->uc_stack.ss_flags = 0; } while (0)

#define set_function_to_run_in(uctx, func)          makecontext((uctx), &(func), 0)


namespace rubinius {

  /* Forwards */
  class Task;
  class Message;
  class NativeMethod;


  /**
   *  Method defined in a C extension.
   */
  class VMNativeMethod : public VMExecutable
  {
  public:   /* Ctors */

  public:   /* Interface */

                  /**
                   *  Call the C function.
                   *
                   *  We grab the information needed from the active context, convert
                   *  everything necessary to handles, and then directly call the C
                   *  function with those parameters. Eventually the return value is
                   *  saved in the context and we jump back to the dispatch point.
                   *
                   *  (It is not possible to simply return since this method is in
                   *  a different stack from the dispatcher.)
                   */
    static void   perform_call();

                  /**
                   *  Handle C method call including its callstack.
                   *
                   *  In addition to setting up the NativeMethodContext for the call
                   *  and the call itself (arguments, return value and all), we also
                   *  handle setting up further calls from the method to other Ruby
                   *  or C methods.
                   *
                   *  @note   Shamelessly tramples over the standard VMExecutable@execute.
                   */
    static bool   execute(VM* state, Task* task, Message* message, NativeMethod* method);

  };

}

#endif

