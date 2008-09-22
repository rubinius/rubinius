#ifndef RBX_BUILTIN_NATIVEMETHODCONTEXT_HPP
#define RBX_BUILTIN_NATIVEMETHODCONTEXT_HPP

/* Std */
#include <vector>

/* Sys */
#include <ucontext.h>

/* Project */
#include "type_info.hpp"
#include "builtin/contexts.hpp"


namespace rubinius {

  /* Forwards */
  class Message;
  class NativeMethod;
  class Task;


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
          my_storage->push_back(obj);
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
   *  TODO: Subclassing MethodContext is fucking ugly.
   *        Create a Context superclass for both.
   *
   *  TODO: Add tests.
   */
  class NativeMethodContext : public MethodContext {
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

                                  /** 64KiB -- Do NOT decrease this, ucontext will fail. */
       static const std::size_t   DEFAULT_STACK_SIZE = 1024 * 64;


    public:   /* Ctors */

                                  /**
                                   *  Brand new context for a brand new call.
                                   *
                                   *  The C stack is allocated *inside* the context.
                                   */
    static NativeMethodContext*   create(VM* state, Message* message, Task* task, NativeMethod* method);


    public:   /* Interface */

                                  /** Record a NativeMethodContext as the currently active one. */
                   static void    current_context_is(NativeMethodContext* context);
                                  /** Access currently active NativeMethodContext. */
    static NativeMethodContext*   current();


    public:   /* Accessors */

      Action action() { return my_action; }
      NativeMethod* method() { return my_method; }
      HandleStorage& handles() { return my_handles; }
      Message* message() { return my_message; }

      VM* state() { return my_state; }

      void action(Action action) { my_action = action; }
      void return_value(Object* obj) { my_return_value = obj; }
      Object* return_value() { return my_return_value; }

      std::size_t stacksize() { return stack_size; }
      void* stack() { return static_cast<void*>(stk); }

      ucontext_t* c_call_point() { return &my_c_call_point; }
      ucontext_t* dispatch_point() { return &my_dispatch_point; }

    private:  /* Hidden stuff. */

                                  /** Constructor allocates the NMC, hidden behind ::create(). */
                                  NativeMethodContext(VM* state, Message* message, Task* task, NativeMethod* method);



    public:   /* Bootstrapping */

                                  /** Add the class object to the object space. */
                   static void    register_class_with(VM* state);


    public:   /* Slots and bookkeeping. */

        const static size_t   fields  = 0;      // TODO: Check this
    const static object_type  type    = NativeMethodContextType;


    /*  OK, these are the slots we get from MethodContext (and all we really need.)

     *  MethodContext* sender_; // slot
     *  MethodContext* home_;   // slot
     *  OBJECT self_;           // slot

     *  CompiledMethod* cm_;    // slot

     *  Module* module_;        // slot
     *  OBJECT block_;          // slot
     *  OBJECT name_;           // slot
     */


    public:  /* Instance vars */

    /*  These are the inherited ivars, of which the two last are
     *  useful for our purposes. We are going to abuse the hell
     *  out of the "array": the object is allocated the extra
     *  memory at the end, so the variable just works as a pointer
     *  to that region.
     *
     *  VMMethod* vmm;

     *  struct jit_state js;
     *  int    ip;
     *  size_t args;

     *  size_t stack_size;
     *
     *  OBJECT stk[];
     */

    /*
     *  OS X 10.5's ucontext implementation is broken and requires
     *  that we manually set up the additional mcontext fields so
     *  that the ucontext_t's are sized up correctly. Otherwise they
     *  get written over :/
     */

    /* TODO: Make this an actual conditional and stuff. */

#ifndef HAS_UCONTEXT
  #define HAS_UCONTEXT
#endif

    Action          my_action;            /**< Action requested to be performed. */
    ucontext_t      my_c_call_point;      /**< Point to execute actual C dispatch (subtend stack) */
#if defined(__APPLE__) && defined(HAS_UCONTEXT)
  _STRUCT_MCONTEXT  __my_c_call_point_mc;
#endif
    ucontext_t      my_dispatch_point;    /**< Point of return to dispatch code (vm stack) */
#if defined(__APPLE__) && defined(HAS_UCONTEXT)
  _STRUCT_MCONTEXT  __my_dispatch_point_mc;
#endif
    Message*        my_message;           /**< Message representing this call. */
    NativeMethod*   my_method;            /**< Function-like object that actually implements the method. */
    HandleStorage   my_handles;           /**< Object handles for this call. */
    Object*         my_return_value;      /**< Return value from the call. */
//    char*           my_stack;
//    std::size_t     my_stack_size;
    VM*             my_state;             /**< VM state for this invocation. */
    Task*           my_task;              /**< Task in which we are running. */


  public:

    /**
     *  Generated type information.
     */
    class Info : public MethodContext::Info
    {
      public:
      BASIC_TYPEINFO(MethodContext::Info)
    };
  };

}


#endif

