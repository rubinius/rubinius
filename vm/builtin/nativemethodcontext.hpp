#ifndef RBX_BUILTIN_NATIVEMETHODCONTEXT_HPP
#define RBX_BUILTIN_NATIVEMETHODCONTEXT_HPP

/* Std */
#include <sstream>
#include <vector>

/* Project */
#include "quantum_stack_leap.hpp"
#include "type_info.hpp"

#include "vm/object_utils.hpp"

#include "builtin/contexts.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"


namespace rubinius {

  /* Forwards */
  class Message;
  class NativeMethod;


  /** Theoretically this could be changed to some other storage.. */
  typedef std::vector<Object*> HandleStorage;

  /** More prosaic name for Handles. */
  typedef intptr_t Handle;


  /**
  *   Method context for C-implemented methods.
  *
  *   @todo Add @current_line that can be updated by e.g.
  *         macroed rb_funcall.
  *
  *   @todo Better Handle indexing? Currently there are
  *         sizeof(size_t) / 2 regular handles available
  *         because negative indices point to globals.
  *
  *   @todo Figure out how to handle Messages in dup/clone.
  *
  *   @todo Subclassing MethodContext is fucking ugly.
  *         Create a Context superclass for both.
  *
  *   @todo Clarify terminology for vm->subtend->vm stuff.
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
      ERROR_RAISED,
      SEGFAULT_DETECTED
    };


  public:   /* Constants */

    /** 64KiB -- Do NOT decrease this, ucontext will fail. */
    static const std::size_t DEFAULT_STACK_SIZE = 1024 * 64;


  public:   /* Ctors */

    /** Create and populate context for a new C call. */
    static NativeMethodContext* create(VM* state,
                                       Message* msg = NULL,
                                       Task* task = as<Task>(Qnil),
                                       NativeMethod* method = reinterpret_cast<NativeMethod*>(Qnil));

    /** Allocate a functional but empty context. */
    static NativeMethodContext* allocate(VM* state);


  public:   /* Class interface */

    /** Record a NativeMethodContext as the currently active one. */
    static void                 current_context_is(NativeMethodContext* context);

    /** Access currently active NativeMethodContext. */
    static NativeMethodContext* current();

    /** Global handles. @todo Concurrency. */
    static HandleStorage&       global_handles();


  public:   /* Accessors */


    Action          action()                    const { return action_; }
    void            action(Action action)             { action_ = action; }
    ExecutionPoint  c_call_point()                    { return &c_call_point_; }
    ExecutionPoint  dispatch_point()                  { return &dispatch_point_; }
    HandleStorage&  handles()                         { return *handles_; }
    ExecutionPoint  inside_c_method_point()           { return &inside_c_method_point_; }
    Message*        message()                   const { return message_; }
    Message&        message_from_c()                  { return *message_from_c_; }
    NativeMethod*   method()                    const { return method_; }
    Object*         return_value()              const { return return_value_; }
    void            return_value(Object* obj)         { return_value_ = obj; }
    std::size_t     stacksize()                 const { return stack_size; }
    void*           stack()                           { return stack_; }
    VM*             state()                     const { return state_; }
    Task*           task()                      const { return task_; }
    Handle          value_returned_to_c()       const { return c_return_value_; }
    void            value_returned_to_c(Object* obj)  { c_return_value_ = handle_for(obj); }


  public:   /* Interface */

//    /** Create new context using this one's data and internal state. @see dup(). */
//    // Ruby.primitive :nativemethodcontext_clone
//    Object*     clone(STATE);
//
//    /** Create a new context using this one's data but _not_ internal state. @see clone(). */
//    // Ruby.primitive :nativemethodcontext_dup
//    Object*     dup(STATE);

    /** Generate a handle to refer to the given object from C */
    Handle      handle_for(Object* obj);

    /** Generate a global handle to refer to the given object from C */
    Handle      handle_for_global(Object* obj);

    /** Mark handles for GC, including the global handle set. */
    void        mark_handles(ObjectMark& mark);

    /** Get the object corresponding to this handle. */
    Object*     object_from(Handle handle);

    /** Get the object corresponding to this global handle. */
    Object*     object_from_global(Handle handle);


  private:  /* Instance vars */

    /* We are ignoring MethodContext's stk */

    /** Execution point where actual function call made (Subtend stack.) */
    DECLARE_POINT_VARIABLE(c_call_point_);

    /** Execution point to return to for further handling (VM stack.) */
    DECLARE_POINT_VARIABLE(dispatch_point_);

    /** Execution point to return to from having made a call from a C method. */
    DECLARE_POINT_VARIABLE(inside_c_method_point_);

    Action          action_;            /**< Action for the VMNativeMethod to perform. */
    Handle          c_return_value_;    /**< Return value for a call back to VM. */
    Message*        message_;           /**< Message representing this call. */
    Message*        message_from_c_;    /**< Message for calls back from the method. */
    NativeMethod*   method_;            /**< Function-like object that actually implements the method. */
    HandleStorage*  handles_;           /**< Object handles for this call. */
    Object*         return_value_;      /**< Return value from the call. */
    void*           stack_;             /**< Stack for executing the C method. */
    VM*             state_;             /**< VM state for this invocation. */
    Task*           task_;              /**< Task in which we are running. */


  public:

    /** Generated type information. */
    class Info : public MethodContext::Info
    {
    public:
      BASIC_TYPEINFO_WITH_CLEANUP(MethodContext::Info)

      virtual void mark(Object* self, ObjectMark& mark);
      virtual void show(STATE, Object* self, int level);
    };

  };    /* NativeMethodContext */

}


#endif

