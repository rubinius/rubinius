#ifndef RBX_BUILTIN_NATIVEMETHODCONTEXT_HPP
#define RBX_BUILTIN_NATIVEMETHODCONTEXT_HPP

/* Std */
#include <vector>

/* Project */
#include "quantum_stack_leap.hpp"

#include "type_info.hpp"
#include "builtin/contexts.hpp"


namespace rubinius {

  /* Forwards */
  class Message;
  class NativeMethod;
  class Task;


  /** Theoretically this could be changed to some other storage.. */
  typedef std::vector<Object*> HandleStorage;

  /** More prosaic name for Handles. */
  typedef std::size_t Handle;


  /**
  *  Method context for C-implemented methods.
  *
  *  TODO: Subclassing MethodContext is fucking ugly.
  *        Create a Context superclass for both.
  *
  *  TODO: Add tests.
  *
  *  TODO: Clarify terminology for vm->subtend->vm stuff.
  */
  class NativeMethodContext : public MethodContext {
  public:   /* Slots and bookkeeping. */

    /** Dynamically calculated size. (TODO) */
    const static std::size_t fields = 0;
    const static object_type type = NativeMethodContextType;

    /** Add the class object to the object space. */
    static void register_class_with(VM* state);


  public:   /* Types */

    /**
     *  Control flow requests.
     *
     *  Due to the unusual program flow here, the context will
     *  explicitly contain the action that the dispatch code
     *  should perform for this particular context/call.
     */
    enum Action {
      UNDEFINED,
      ORIGINAL_CALL,
      CALL_FROM_C,
      RETURNED_BACK_TO_C,
      RETURN_FROM_C,
    };


  public:   /* Constants */

    /** 64KiB -- Do NOT decrease this, ucontext will fail. */
    static const std::size_t DEFAULT_STACK_SIZE = 1024 * 64;


  public:   /* Ctors */

    /** Create and populate context for a new C call. */
    static NativeMethodContext* create(VM* state, Message* message, Task* task, NativeMethod* method);

    /** Allocate a functional but empty context. */
    static NativeMethodContext* allocate(VM* state);


  public:   /* Accessors */

    VM* state() { return my_state; }

    Action action() { return my_action; }
    void action(Action action) { my_action = action; }

    NativeMethod* method() { return my_method; }
    HandleStorage& handles() { return *my_handles; }
    Message* message() { return my_message; }

    Message& message_from_c() { return *my_message_from_c; }

    void return_value(Object* obj) { my_return_value = obj; }
    Object* return_value() { return my_return_value; }

    std::size_t stacksize() { return stack_size; }
    void* stack() { return my_stack; }

    Task* task() { return my_task; }

    void value_returned_to_c(Handle obj) { my_c_return_value = obj; }
    Handle value_returned_to_c() { return my_c_return_value; }

    ExecutionPoint c_call_point() { return &my_c_call_point; }
    ExecutionPoint dispatch_point() { return &my_dispatch_point; }
    ExecutionPoint inside_c_method_point() { return &my_inside_c_method_point; }


  public:   /* Interface */

    /** Record a NativeMethodContext as the currently active one. */
    static void current_context_is(NativeMethodContext* context);

    /** Access currently active NativeMethodContext. */
    static NativeMethodContext* current();

    /** Get the object corresponding to this handle. */
    Object* object_from(Handle handle);

    /** Generate a handle to refer to the given object from C */
    Handle handle_for(Object* obj);


  public:  /* Instance vars */

    /* We are ignoring MethodContext's stk */

    /** Execution point where actual function call made (Subtend stack.) */
    DECLARE_POINT_VARIABLE(my_c_call_point);

    /** Execution point to return to for further handling (VM stack.) */
    DECLARE_POINT_VARIABLE(my_dispatch_point);

    /** Execution point to return to from having made a call from a C method. */
    DECLARE_POINT_VARIABLE(my_inside_c_method_point);

    Action          my_action;            /**< Action for the VMNativeMethod to perform. */
    Handle          my_c_return_value;    /**< Return value for a call back to VM. */
    Message*        my_message;           /**< Message representing this call. */
    Message*        my_message_from_c;    /**< Message for calls back from the method. */
    NativeMethod*   my_method;            /**< Function-like object that actually implements the method. */
    HandleStorage*  my_handles;           /**< Object handles for this call. */
    Object*         my_return_value;      /**< Return value from the call. */
    void*           my_stack;             /**< Stack for executing the C method. */
    VM*             my_state;             /**< VM state for this invocation. */
    Task*           my_task;              /**< Task in which we are running. */


  public:

    /** Generated type information. */
    class Info : public MethodContext::Info
    {
    public:
      BASIC_TYPEINFO_WITH_CLEANUP(MethodContext::Info)
    };

  };    /* NativeMethodContext */

}


#endif

