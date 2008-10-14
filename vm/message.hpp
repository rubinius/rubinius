#ifndef RBX_MESSAGE_HPP
#define RBX_MESSAGE_HPP

#include "prelude.hpp"

namespace rubinius {

  class Array;
  class Executable;
  class Module;
  class SendSite;
  class Task;
  class TypeInfo;

  /**
   *  Message represents a single unique method call.
   *
   *  Every time that a method call occurs in a running program, a new
   *  Message object is constructed. A Message represents that particular
   *  unique call. In the send_* instruction, information added to the
   *  Message includes the SendSite, the receiver, the arguments as well
   *  as the Task. In the method lookup process, the located Executable
   *  object and the Module in which it was found are added. Eventually,
   *  a MethodContext object is constructed using the information in the
   *  Message. See instance variable documentation for all of the info
   *  gathered in a Message.
   *
   *  @see  SendSite
   *  @see  MethodContext
   */
  class Message {
  public:

    Message(STATE, Array* ary);
    Message(STATE);

  public:   /* Interface */

    /**
     *  Number of remaining (unconsumed) arguments.
     */
    size_t args() { return total_args - start_; }

    /**
     *  Appends the task's arguments to the splat array.
     */
    void append_arguments(STATE, Task* task, Array* splat);

    /**
     *  Appends splat arguments to the task's arguments array.
     */
    void append_splat(STATE, Task* task, Array* splat);

    /**
     *  Argument at offset given from current start.
     *
     *  Note that this is the *current* start, in the case
     *  where arguments have already been consumed.
     */
    Object* get_argument(size_t index);

    /**
     *  Copy arguments from stack into arguments Array.
     */
    void import_arguments(STATE, Task* task, size_t args, size_t offset = 0);

    /**
     *  Drop arguments Array.
     */
    void reset() {
      start_ = 0;
      arguments = NULL;
      method_missing = false;
    }

    /**
     *  Explicitly set total number of arguments.
     */
    void set_args(size_t count) { total_args = count; }

    /**
     *  Explicitly set argument Array to the one given.
     */
    void set_arguments(STATE, Array* args);

    /**
     *  Remove and return the currently first remaining argument.
     */
    Object* shift_argument(STATE);

    /**
     *  Insert argument to the front of arguments and bump others back by one.
     */
    void unshift_argument(STATE, Object* val);

    /**
     *  Insert 2 arguments to the front of arguments and bump others back.
     */
    void unshift_argument2(STATE, OBJECT one, OBJECT two);

    /**
     *  Set the associated Task and explictly give total argument count.
     */
    void use_from_task(Task* task, size_t args);

    /*
     * Package up the arguments and return them as an Array
     */
    Array* as_array(STATE);

  private:
    STATE    /* state */;       /**< Access to the VM state. */
    Array*      arguments;      /**< Arguments from the call. */
    size_t      total_args;     /**< Total number of arguments given, including unsplatted. */
    size_t      start_;          /**< Index of first remaining argument in arguments. */

  public:   /* Instance variables */

    SendSite*   send_site;      /**< SendSite in which this call originates. */
    SYMBOL      name;           /**< Name of the method being called (comes from SendSite) */
    Object*     recv;           /**< Receiver in the call, i.e. obj in `obj.foo()` */
    Object*     block;          /**< Block object or nil if no block. */
    Object*     splat;          /**< NOT USED. The splat argument to the call. */
    Object*     current_self;   /**< self at the point of the call. */
    size_t      stack;          /**< Number of arguments on the stack when call occurs + 1 for return value. */
    bool        priv;           /**< Indicates that this call can access private methods. */

    Module*     lookup_from;    /**< The Module in which is the first method table to look from. Usually MetaClass or Class. */
    Executable* method;         /**< Executable, i.e. method object, that will be run. Added in method lookup. */
    Module*     module;         /**< Module in which the method object was found. Added in method lookup. */

    /**
     * Indicates if we're calling a method setup as method_missing.
     * This means we need to unshift the name of the name the method
     * was called as. */
    bool        method_missing;

  private:
    /** The caller's MethodContext, where to get arguments from*/
    MethodContext* caller;


  public: // accessors
    size_t start() { return start_; }
  };
}

#endif

