#ifndef RBX_MESSAGE_HPP
#define RBX_MESSAGE_HPP

#include "prelude.hpp"
#include "builtin/array.hpp"
#include "builtin/contexts.hpp"
#include "builtin/tuple.hpp"

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

    Message(STATE);
    Message(STATE, Array* ary);

  public:   /* Interface */

    /**
     *  The number of arguments available
     */
    size_t args() { return total_args; }

    /**
     *  Appends the task's arguments to the splat array.
     */
    void append_arguments(STATE, Array* splat);

    /**
     *  Appends splat arguments to the task's arguments array.
     */
    void append_splat(STATE, Array* splat);

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

    /*
     * Package up the arguments and return them as an Array
     */
    Array* as_array(STATE);

    /*
     * Returns the object that is currently self
     */
    OBJECT current_self();

    /*
     * Sets the caller context
     */
    void set_caller(MethodContext* ctx) {
      caller = ctx;
    }

    /*
     * Setup the Message with the basic information
     */
    void setup(SendSite* ss, OBJECT obj, MethodContext* ctx, size_t arg_count,
        size_t stack_size) {
      method_missing = false;
      arguments_array = NULL;
      send_site = ss;
      recv   = obj;
      caller = ctx;
      total_args   = arg_count;
      stack  = stack_size;
      stack_args_ = ctx->stack_back_position(arg_count - 1);
      arguments_ = stack_args_;
    }

    /*
     * Retrieve an argument from the stack
     */
    OBJECT get_stack_arg(int which) {
      return stack_args_[which];
    }

    /*
     * Shift the start of the arguments forward, discarding the top
     * argument. */
    OBJECT shift_stack_args() {
      OBJECT obj = *stack_args_++;
      arguments_ = stack_args_;
      return obj;
    }

    /*
     * Sets the Message to pull it's arguments from Array*
     */
    void use_array(Array* ary) {
      total_args = ary->size();
      arguments_array = ary;
      arguments_ = ary->tuple()->field + ary->start()->to_native();
    }

    /*
     * Retrieve the requested argument
     */
    OBJECT get_argument(size_t index) {
      return arguments_[index];
    }

    /*
     * Deprecated: Use the details to setup the Message
     */
    void use_from_task(Task* task, size_t args);

  private:
    STATE    /* state */;       /**< Access to the VM state. */
    Array*      arguments_array;      /**< Arguments from the call. */
    size_t      total_args;     /**< Total number of arguments given, including unsplatted. */
    OBJECT*     stack_args_;
    OBJECT*     arguments_;

  public:   /* Instance variables */

    SendSite*   send_site;      /**< SendSite in which this call originates. */
    SYMBOL      name;           /**< Name of the method being called (comes from SendSite) */
    Object*     recv;           /**< Receiver in the call, i.e. obj in `obj.foo()` */
    Object*     block;          /**< Block object or nil if no block. */
    Object*     splat;          /**< NOT USED. The splat argument to the call. */
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

  };
}

#endif

