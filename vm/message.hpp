#ifndef RBX_MESSAGE_HPP
#define RBX_MESSAGE_HPP

#include "prelude.hpp"
#include "builtin/array.hpp"
#include "call_frame.hpp"
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
   *  a CallFrame object is constructed using the information in the
   *  Message. See instance variable documentation for all of the info
   *  gathered in a Message.
   *
   *  @see  SendSite
   *  @see  CallFrame
   */
  class Message {
  public:
    /* Default constructor that initializes everything for safety.
     * You probably want one of the specialized constructors below.
     */
    Message::Message();

    /* Constructer used by all send_* instructions */
    Message(STATE, SendSite* ss, Symbol* name, Object* recv, CallFrame* call_frame,
            size_t arg_count, Object* block, bool priv, Module* lookup_from);

    /* Constructor used by e.g. Helper::const_missing */
    Message(STATE, Symbol* name, Object* recv, size_t arg_count,
            Object* block, Module* lookup_from);

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
    void unshift_argument2(STATE, Object* one, Object* two);

    /*
     * Package up the arguments and return them as an Array
     */
    Array* as_array(STATE);

    /*
     * Returns the object that is currently self
     */
    Object* current_self();

    /*
     * Sets the caller context
     */
    void set_caller(CallFrame* call_frame) {
      caller_ = call_frame;
    }

    CallFrame* caller() {
      return caller_;
    }

    /*
     * Retrieve an argument from the stack
     */
    Object* get_stack_arg(int which) {
      return stack_args_[which];
    }

    /*
     * Shift the start of the arguments forward, discarding the top
     * argument. */
    Object* shift_stack_args() {
      Object* obj = *stack_args_++;
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
    Object* get_argument(size_t index) {
      return arguments_[index];
    }

    /*
     * Deprecated: Use the details to setup the Message
     */
    void use_from_task(Task* task, size_t args);

    Object* send(STATE, CallFrame* call_frame);

  private:
    STATE       /* state */;       /**< Access to the VM state. */
    Array*      arguments_array;      /**< Arguments from the call. */
    size_t      total_args;     /**< Total number of arguments given, including unsplatted. */
    Object**    stack_args_;
    Object**    arguments_;

  public:   /* Instance variables */

    SendSite*   send_site;      /**< SendSite in which this call originates. */
    Symbol*     name;           /**< Name of the method being called (comes from SendSite) */
    Object*     recv;           /**< Receiver in the call, i.e. obj in `obj.foo()` */
    Object*     block;          /**< Block object or nil if no block. */
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
    /** The caller's CallFrame, where to get arguments from*/
    CallFrame* caller_;

  };
}

#endif

