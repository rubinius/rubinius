#ifndef RBX_MESSAGE_HPP
#define RBX_MESSAGE_HPP

#include "prelude.hpp"
#include "builtin/array.hpp"
#include "call_frame.hpp"
#include "builtin/tuple.hpp"

#include "arguments.hpp"

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
    Arguments args_;

    /** The caller's CallFrame, where to get arguments from*/
    CallFrame* caller_;

    /**< The Module in which is the first method table to look from.
     * Usually MetaClass or Class. If NULL, then when requested, calculated
     * from recv */
    Module* lookup_from_;

  public:   /* Instance variables */

    SendSite*   send_site;      /**< SendSite in which this call originates. */
    Symbol*     name;           /**< Name of the method being called (comes from SendSite) */
    Object*     recv;           /**< Receiver in the call, i.e. obj in `obj.foo()` */
    Object*     block;          /**< Block object or nil if no block. */
    bool        priv;           /**< Indicates that this call can access private methods. */

    Executable* method;         /**< Executable, i.e. method object, that will be run. Added in method lookup. */
    Module*     module;         /**< Module in which the method object was found. Added in method lookup. */

    /**
     * Indicates if we're calling a method setup as method_missing.
     * This means we need to unshift the name of the name the method
     * was called as. */
    bool        method_missing;

  public:
    Message()
      : method_missing(false)
    {}

    /* Constructer used by all send_* instructions */
    Message(SendSite* ss, Symbol* name, Object* recv, CallFrame* call_frame,
            size_t arg_count, Object* block, bool priv, Module* lookup_from = 0);

    /* Constructor used by e.g. Helper::const_missing */
    Message(Symbol* name, Object* recv, size_t arg_count,
            Object* block, Module* lookup_from = 0);

    Message(STATE);
    Message(Array* ary);
    Message(CallFrame* call_frame, size_t arg_count);

  public:   /* Interface */

    Arguments& arguments() {
      return args_;
    }

    /**
     *  The number of arguments available
     */
    size_t args() { return args_.total(); }

    /*
     * Retrieve the requested argument
     */
    Object* get_argument(size_t index) {
      return args_.get_argument(index);
    }

    /**
     *  Explicitly set argument Array to the one given.
     */
    void set_arguments(STATE, Array* args);

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

    /**
     *  Send this message directly.
     *
     *  This is separate from the SendSite performers, because
     *  there is no send site to use.
     *
     *  @todo See whether there is any point to having "virtual"
     *        send sites (which also addresses this.) --rue
     *
     *  @see  SendSite
     */
    Object* send(STATE, CallFrame* call_frame);

    Module* lookup_from(STATE);

    void set_lookup_from(Module* mod) {
      lookup_from_ = mod;
    }

    /**
     * Reset the Message to it's pre-lookup state
     */
    void flush_lookup();

  };
}

#endif

