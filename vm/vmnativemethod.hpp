#ifndef RBX_VMNATIVEMETHOD_HPP
#define RBX_VMNATIVEMETHOD_HPP

/* Std */
#include <vector>

/* System */
#include <ucontext.h>

/* Project */
#include "builtin/task.hpp"
#include "vmexecutable.hpp"


#define HAS_UCONTEXT 1

/* Some alterable labels to create an API. Sadly #defines are a must here. */
#define store_current_execution_point_in(uctx)      getcontext(&(uctx))
#define jump_to_execution_point_in(uctx)            setcontext(&(uctx))

#define create_execution_point_with_stack(uctx, stack, size) do { getcontext(&(uctx)); \
                                                                  (uctx).uc_link = NULL; \
                                                                  (uctx).uc_stack.ss_sp = (stack); \
                                                                  (uctx).uc_stack.ss_size = (size); \
                                                                  (uctx).uc_stack.ss_flags = 0; } while (0)

#define set_function_to_run_in(uctx, func)          makecontext(&(uctx), &(func), 0)


namespace rubinius {

  /* Forwards */
  class NativeMethod;

  typedef std::vector<Object*> HandleStorage;

  /**
   *  Handle to an Object for C methods.
   *
   *  Handles give an extra layer of abstraction to Object handling
   *  in C extensions. The main motivation is the GC: it must be
   *  able to move Objects around which would cause problems for
   *  the extension programmer.
   *
   *  Handles always refer to the objects as Objects, rather than
   *  the actual class. All objects are upcast to Object*s.
   *
   *  Handles should be used as values only (the copy semantics
   *  are correct.) Pointers to Handles should be unnecessary.
   *
   *  TODO: Add tests.
   *  TODO: Avoid extra work on Qnil/Qtrue/Qfalse.
   */
  class Handle
  {
    public:   /* Ctors */

      Handle() : my_index(0), my_storage(NULL) {}

      template <typename T>
        Handle(HandleStorage& storage, T* obj)
          : my_index(0)
          , my_storage(&storage)
        {
          my_storage->push_back(as<Object>(obj));
          my_index = my_storage->size() - 1;
        }

      Handle(const Handle& other)
        : my_index(other.my_index)
        , my_storage(other.my_storage)
      {}


    public:   /* Interface */

                /** Explicitly retrieve the contained object. */
       Object*  object() { return (*my_storage)[my_index]; }


    private:  /* Instance vars */

      std::size_t     my_index;     /**< Index into storage to retrieve Object. */
      HandleStorage*  my_storage;   /**< Storage of actual object information. */
  };

  typedef Handle HandleTo;

  /**
   *  Specialised version of as() to access a Handle's actual object.
   *
   *  @see  vm/object.hpp for normal version.
   */
  template <typename T>
    T* as(HandleTo handle) { return as<T>(handle.object()); }

  /**
   *  Specialised version of try_as() to access a Handle's actual object.
   *
   *  @see  vm/object.hpp for normal version.
   */
  template <typename T>
    T* try_as(HandleTo handle) { return try_as<T>(handle.object()); }


  /**
   *  Method context for C-implemented methods.
   *
   *  TODO: Add tests.
   */
  class NativeMethodContext
  {
    public:   /* Types */

    /**
     *  Control flow requests.
     *
     *  Due to the unusual program flow here, the context will
     *  explicitly contain the action that the dispatch code
     *  should perform for this particular context/call.
     *
     */
    enum Action
    {
      UNDEFINED,
      ORIGINAL_CALL,
      CALL_FROM_C,
      RETURN_FROM_C,
    };


    public:   /* Constants */

       static const std::size_t   DEFAULT_STACK_SIZE = 65536;   /* 64Kib */


    public:   /* Ctors */

                                  /** Brand new context for a brand new call. */
    static NativeMethodContext*   create(Message* message, Task* task, NativeMethod* method);


    public:   /* Interface */

                                  /** Record a NativeMethodContext as the currently active one. */
                   static void    current_context_is(NativeMethodContext* context);
                                  /** Access currently active NativeMethodContext. */
    static NativeMethodContext*   current();


    public:  /* Instance vars */

    /*
     *  OS X 10.5's ucontext implementation is broken and requires
     *  that we manually set up the additional mcontext fields so
     *  that the ucontext_t's are sized up correctly. Otherwise they
     *  get written over :/
     */

    Action          action;           /**< Action requested to be performed. */
    ucontext_t      c_call_point;     /**< Point to execute actual C dispatch (subtend stack) */
#if defined(__APPLE__) && defined(HAS_UCONTEXT)
  _STRUCT_MCONTEXT  __c_call_point_mc;
#endif
    ucontext_t      dispatch_point;   /**< Point of return to dispatch code (vm stack) */
#if defined(__APPLE__) && defined(HAS_UCONTEXT)
  _STRUCT_MCONTEXT  __dispatch_point_mc;
#endif
    Message*        message;          /**< Message representing this call. */
    NativeMethod*   method;           /**< Function-like object that actually implements the method. */
    HandleStorage   handles;          /**< Object handles for this call. */
    Object*         return_value;     /**< Return value from the call. */
    MethodContext*  sender;           /**< Context in which this call was made. */
    char*           stack;            /**< Memory area to be used as the stack. */
    std::size_t     stack_size;       /**< Size of the memory area to be used as the stack. */
    VM*             state;            /**< VM state for this invocation. */
    Task*           task;             /**< Task in which we are running. */
  };


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

