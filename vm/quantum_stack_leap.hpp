#ifndef RBX_STACK_JUMPER_HPP
#define RBX_STACK_JUMPER_HPP

#include "detection.hpp"

/** API to hide stack jump mechanism. */

#define RBX_SUBTEND_BY_UCONTEXT 1

#ifdef RBX_SUBTEND_BY_UCONTEXT

  #include "subtend/PortableUContext.h"

  #define HAS_UCONTEXT 1

  /*
   *  OS X 10.5's ucontext implementation is broken and requires
   *  that we manually set up the additional mcontext fields so
   *  that the ucontext_t's are sized up correctly. Otherwise they
   *  get written over :/
   */
  #ifdef OS_X_10_5
    #define DECLARE_POINT_VARIABLE(name) ucontext_t name; \
                                         _STRUCT_MCONTEXT __ ## name ## _mc
  #else
    #define DECLARE_POINT_VARIABLE(name) ucontext_t name
  #endif

  /** Externally used type name for the execution point. */
  typedef ucontext_t* ExecutionPoint;

  /* Some alterable labels to create an API. Sadly #defines are a must here. */
  #define store_current_execution_point_in(uctx)      getcontext((uctx))

  #define jump_to_execution_point_in(uctx)            setcontext((uctx))
  
  #ifdef OS_X_10_4
    #define create_execution_point_with_stack(uctx, stack, size) do { \
      getcontext((uctx)); \
      (uctx)->uc_link = NULL; \
      (uctx)->uc_stack.ss_flags = 0; \
      (uctx)->uc_stack.ss_sp = (char *)(stack); \
      (uctx)->uc_stack.ss_size = (size); } while (0)
  #else
    #define create_execution_point_with_stack(uctx, stack, size) do { \
      getcontext((uctx)); \
      (uctx)->uc_stack.ss_sp = (char *)(stack); \
      (uctx)->uc_stack.ss_size = (size); } while (0)
  #endif

  #define set_function_to_run_in(uctx, func)          makecontext((uctx), &(func), 0)

#endif  /* RBX_SUBTEND_BY_UCONTEXT */


#endif

