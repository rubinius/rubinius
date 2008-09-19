#ifndef RBX_BUILTIN_NATIVEMETHODCONTEXT_HPP
#define RBX_BUILTIN_NATIVEMETHODCONTEXT_HPP

/* Std */
#include <vector>

/* Project */
#include "type_info.hpp"
#include "builtin/contexts.hpp"

#include "vmnativemethod.hpp"


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
  class Handle {
  public:   /* Ctors */

    /** Blank creation needed by HandleStorages. */
    Handle();

    /**
     *  Normal creation to associate any Object subclass.
     *
     *  Objects always stored as Objects, so user is resposible
     *  for casting to the correct type when needed.
     */
    template <typename T>
      Handle::Handle(HandleStorage& storage, T* obj)
        : my_index(0)
        , my_storage(&storage)
      {
        my_storage->push_back(as<Object>(obj));
        my_index = my_storage->size() - 1;
      }

    /** Copy construction semantics for Handle locals. */
    Handle(const Handle& other);


  public:   /* Interface */

    /** Explicitly retrieve the contained object. */
    Object*  object();


  private:  /* Instance vars */

    std::size_t     my_index;     /**< Index into storage to retrieve Object. */
    HandleStorage*  my_storage;   /**< Storage of actual object information. */
  };

  /** More prosaic name for Handles. */
  typedef Handle HandleTo;


  /**
  *  Specialised version of as() to access a Handle's actual object.
  *
  *  @see  vm/object.hpp for normal version.
  */
  template <typename T>
    T* as(HandleTo handle)
    {
      return as<T>(handle.object());
    }


  /**
  *  Specialised version of try_as() to access a Handle's actual object.
  *
  *  @see  vm/object.hpp for normal version.
  */
  template <typename T>
    T* try_as(HandleTo handle)
    {
      return try_as<T>(handle.object());
    }


  /**
  *  Method context for C-implemented methods.
  *
  *  TODO: Subclassing MethodContext is fucking ugly.
  *        Create a Context superclass for both.
  *
  *  TODO: Add tests.
  */
  class NativeMethodContext : public MethodContext {
  public:   /* Slots and bookkeeping. */

    /** Dynamically calculated size. (TODO) */
    const static std::size_t fields = 0;
    const static object_type type = NContextType;

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

    void return_value(Object* obj) { my_return_value = obj; }
    Object* return_value() { return my_return_value; }

    std::size_t stacksize() { return stack_size; }
    void* stack() { return my_stack; }

    ExecutionPoint c_call_point() { return &my_c_call_point; }
    ExecutionPoint dispatch_point() { return &my_dispatch_point; }


  public:   /* Interface */

    /** Record a NativeMethodContext as the currently active one. */
    static void current_context_is(NativeMethodContext* context);

    /** Access currently active NativeMethodContext. */
    static NativeMethodContext* current();


  public:  /* Instance vars */

    /* We are ignoring MethodContext's stk */

    /** Execution point where actual function call made (Subtend stack.) */
    DECLARE_POINT_VARIABLE(my_c_call_point);

    /** Execution point to return to for further handling (VM stack.) */
    DECLARE_POINT_VARIABLE(my_dispatch_point);

    Action          my_action;            /**< Action for the VMNativeMethod to perform. */
    Message*        my_message;           /**< Message representing this call. */
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
      BASIC_TYPEINFO(MethodContext::Info)
    };

  };    /* NativeMethodContext */

}


#endif

