#ifndef RBX_BUILTIN_NATIVEMETHOD_HPP
#define RBX_BUILTIN_NATIVEMETHOD_HPP

#include "defines.hpp"
#include "executor.hpp"
#include "memory.hpp"

#include "class/class.hpp"
#include "class/executable.hpp"

#include "diagnostics.hpp"

#include "capi/tag.hpp"

namespace rubinius {
  class ExceptionPoint;
  class Fixnum;
  class Module;
  class NativeMethod;
  class NativeMethodFrame;
  class Pointer;
  class StackVariables;
  class String;

  /**
   * Thread-local info about native method calls. @see NativeMethodFrame.
   */
  class NativeMethodEnvironment {
    ThreadState* state_;

    /** Current callframe in Ruby-land. */
    CallFrame*          current_call_frame_;
    /** Current native callframe. */
    NativeMethodFrame*  current_native_frame_;
    ExceptionPoint*     current_ep_;

    VALUE outgoing_block_;

    diagnostics::MachineMetrics* metrics_;

  public:   /* Class Interface */
    NativeMethodEnvironment(STATE);

    /** Obtain the NativeMethodEnvironment for this thread. */
    static NativeMethodEnvironment* get();

  public:   /* Interface methods */

    diagnostics::MachineMetrics* metrics() {
      return metrics_;
    }

    VALUE outgoing_block() const {
      return outgoing_block_;
    }

    void set_outgoing_block(VALUE value) {
      outgoing_block_ = value;
    }

  public:   /* Accessors */

    Object* block();

    ThreadState* state() {
      return state_;
    }

    CallFrame* current_call_frame() const {
      return current_call_frame_;
    }

    void set_current_call_frame(CallFrame* frame) {
      current_call_frame_ = frame;
    }

    NativeMethodFrame* current_native_frame() const {
      return current_native_frame_;
    }

    void set_current_native_frame(NativeMethodFrame* frame) {
      current_native_frame_ = frame;
    }

    ExceptionPoint* current_ep() const {
      return current_ep_;
    }

    void set_current_ep(ExceptionPoint* ep) {
      current_ep_ = ep;
    }

    StackVariables* scope();
  };


  /**
   *  Call frame for a native method. @see NativeMethodEnvironment.
   */
  class NativeMethodFrame {
    /** the last NativeMethodFrame used up the stack.
     *  @note This is rarely the direct caller. */
    attr_field(previous, NativeMethodFrame*);

    attr_field(env, NativeMethodEnvironment*);

    attr_field(capi_lock_index, int);

    /** Handle for the block passed in **/
    attr_field(block, VALUE);

    /** Handle for receiver **/
    attr_field(receiver, VALUE);

    /** Handle for module method came from **/
    attr_field(module, VALUE);

    /** Handle for method being invoked **/
    attr_field(method, VALUE);

  public:
    NativeMethodFrame(NativeMethodEnvironment* env, NativeMethodFrame* prev, NativeMethod* method);

  public:     /* Interface methods */

    /** Currently active/used Frame in this Environment i.e. thread. */
    static NativeMethodFrame* current() {
      return NativeMethodEnvironment::get()->current_native_frame();
    }

  public:     /* Accessors */

    void setup(VALUE recv, VALUE blk, VALUE meth, VALUE mod) {
      receiver(recv);
      method(meth);
      block(blk);
      module(mod);
    }
  };


  /**
   *  The various special method arities from a C method. If not one of these,
   *  then the number denotes the exact number of args in addition to the
   *  receiver instead.
   */
  enum Arity {
    INIT_FUNCTION = -99,
    ITERATE_BLOCK = -98,
    C_CALLBACK = -97,
    C_LAMBDA = -96,
    C_BLOCK_CALL = -95,
    ARGS_IN_RUBY_ARRAY = -3,
    RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY = -2,
    ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER = -1
  };


  /* The various function signatures needed since C++ requires strict typing here. */

  /** Generic function pointer used to store any type of functor. */

  typedef VALUE (*VariableFunction)(...);

  /* Actual functor types. */

  typedef void (*InitFunction)      (void);   /**< The Init_<name> function. */

  typedef VALUE (*ArgcFunction)     (int, VALUE*, VALUE);


  /**
   *  Ruby side of a NativeMethod, i.e. a C-implemented method.
   *
   *  The possible signatures for the C function are:
   *
   *    VALUE func(VALUE argument_array);
   *    VALUE func(VALUE receiver, VALUE argument_array);
   *    VALUE func(VALUE receiver, int argument_count, VALUE*);
   *    VALUE func(VALUE receiver, ...);    // Some limited number of VALUE arguments
   *
   *  VALUE is an opaque type from which a handle to the actual object
   *  can be extracted. User code never operates on the VALUE itself.
   *
   */
  class NativeMethod : public Executable {
  public:
    attr_accessor(arity, Fixnum);
    attr_accessor(file, String);
    attr_accessor(name, Symbol);
    attr_accessor(module, Module);

  private:
    /** Function object that implements this method. */
    void* func_;
    attr_field(capi_lock_index, int);

  public:
    const static object_type type = NativeMethodType;

    /** Set class up in the VM. @see machine/ontology.cpp. */
    static void bootstrap(STATE);
    static void initialize(STATE, NativeMethod* obj);

    // Called when starting a new native thread, initializes any thread
    // local data.
    static void init_thread(STATE);

    // Called when a thread is exiting, to cleanup the thread local data.
    static void cleanup_thread(STATE);


  public:   /* Ctors */

    /**
     *  Create a NativeMethod object.
     *
     *  Takes a function to call in the form of a void*. +arity+ is used
     *  to figure out how to call the function properly.
     */
    static NativeMethod* create(STATE, String* file_name,
                                Module* module, Symbol* method_name,
                                void* func, Fixnum* arity,
                                int capi_lock_index);

  public:   /* Class Interface */

    /** Set up and call native method. */
    template <class ArgumentHandler>
    static Object* executor_implementation(STATE, Executable* exec, Module* mod,
                                           Arguments& message);

    /**
     * Create a NativeMethod to invoke the function pointed to by +ptr+
     * as a libraries Init function.
     *
     *  A NativeMethod for the function is returned, and can be executed
     *  to actually perform the loading.
     *
     *  Of possibly minor interest, the method's module is set to
     *  Rubinius. It should be updated accordingly when properly
     *  entered.
     */
    // Rubinius.primitive :nativemethod_load_extension_entry_point
    static NativeMethod* load_extension_entry_point(STATE, String* library, Symbol* name, Pointer* ptr);


  public:   /* Instance methods */

    VariableFunction func() {
      return reinterpret_cast<VariableFunction>(func_);
    }

    /** Return the functor cast into the specified type. */
    template <typename FunctionType>
      FunctionType func_as() const {
        return reinterpret_cast<FunctionType>(func_);
      }

  public:   /* Type information */

    /** Type information for NativeMethod. */
    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };

  };    /* NativeMethod */

}

#endif  /* NATIVEMETHOD_HPP */
