#ifndef RBX_BUILTIN_NATIVEMETHOD_HPP
#define RBX_BUILTIN_NATIVEMETHOD_HPP

#include "executor.hpp"
#include "builtin/class.hpp"
#include "builtin/executable.hpp"

#include "capi/tag.hpp"
#include "capi/value.hpp"
#include "capi/handle.hpp"

namespace rubinius {
  class ExceptionPoint;
  class NativeMethodFrame;
  class NativeMethod;
  class Pointer;
  class StackVariables;

  /**
   * Thread-local info about native method calls. @see NativeMethodFrame.
   */
  class NativeMethodEnvironment {
    State state_;

    /** Current callframe in Ruby-land. */
    CallFrame*          current_call_frame_;
    /** Current native callframe. */
    NativeMethodFrame*  current_native_frame_;
    ExceptionPoint*     current_ep_;

    VALUE outgoing_block_;

  public:   /* Class Interface */
    NativeMethodEnvironment(STATE)
      : state_(state->vm())
      , current_call_frame_(0)
      , current_native_frame_(0)
      , current_ep_(0)
      , outgoing_block_(0)
    {}

    /** Obtain the NativeMethodEnvironment for this thread. */
    static NativeMethodEnvironment* get();

  public:   /* Interface methods */

    /** Create or retrieve VALUE for obj. */
    VALUE get_handle(Object* obj);

    /** GC marking for Objects behind VALUEs. */
    void mark_handles(ObjectMark& mark);

  public:
    VALUE outgoing_block() const {
      return outgoing_block_;
    }

    void set_outgoing_block(VALUE val) {
      outgoing_block_ = val;
    }

    /** Obtain the Object the VALUE represents. */
    inline Object* get_object(VALUE val) const {
      if(REFERENCE_P(val)) {
        capi::Handle* handle = capi::Handle::from(val);
        if(!handle->valid_p()) {
          handle->debug_print();
          rubinius::abort();
        }

        return handle->object();
      } else if(FIXNUM_P(val) || SYMBOL_P(val)) {
        return reinterpret_cast<Object*>(val);
      } else if(FALSE_P(val)) {
        return cFalse;
      } else if(TRUE_P(val)) {
        return cTrue;
      } else if(NIL_P(val)) {
        return cNil;
      } else if(UNDEF_P(val)) {
        return cUndef;
      }

      rubinius::bug("requested Object for unknown NativeMethod handle type");
      return cNil; // keep compiler happy
    }


  public:   /* Accessors */

    Object* block();

    State* state() {
      return &state_;
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

    SharedState& shared() {
      return state_.shared();
    }

    /** Set of Handles available in current Frame (convenience.) */
    capi::HandleSet& handles();

    void check_tracked_handle(capi::Handle* hdl, bool need_update=true);

    /** Flush RARRAY, RSTRING, etc. caches, possibly releasing memory. */
    void flush_cached_data();

    /** Updates cached data with changes to the Ruby objects. */
    void update_cached_data();

    StackVariables* scope();
  };


  /**
   *  Call frame for a native method. @see NativeMethodEnvironment.
   */
  class NativeMethodFrame {
    /** the last NativeMethodFrame used up the stack.
     *  @note This is rarely the direct caller. */
    NativeMethodFrame* previous_;

    NativeMethodEnvironment* env_;

    /** HandleSet to Objects used in this Frame. */
    capi::HandleSet handles_;

    int capi_lock_index_;
    bool check_handles_;

    /** Handle for the block passed in **/
    VALUE block_;

    /** Handle for receiver **/
    VALUE receiver_;

    /** Handle for module method came from **/
    VALUE module_;

    /** Handle for method being invoked **/
    VALUE method_;

  public:
    NativeMethodFrame(NativeMethodEnvironment* env, NativeMethodFrame* prev, NativeMethod* method);
    ~NativeMethodFrame();

  public:     /* Interface methods */

    /** Currently active/used Frame in this Environment i.e. thread. */
    static NativeMethodFrame* current() {
      return NativeMethodEnvironment::get()->current_native_frame();
    }

    /** Create or retrieve a VALUE for the Object. */
    VALUE get_handle(STATE, Object* obj);

    void check_tracked_handle(capi::Handle* hdl, bool need_update=true);

    /** Obtain the Object the VALUE represents. */
    Object* get_object(VALUE hndl);

    /** Flush RARRAY, RSTRING, etc. caches, possibly releasing memory. */
    void flush_cached_data();

    /** Updates cached data with changes to the Ruby objects. */
    void update_cached_data();

    int capi_lock_index() const {
      return capi_lock_index_;
    }

  public:     /* Accessors */

    /** HandleSet to Objects used in this Frame. */
    capi::HandleSet& handles() {
      return handles_;
    }

    /** Native Frame active before this call. */
    NativeMethodFrame* previous() const {
      return previous_;
    }

    void setup(VALUE recv, VALUE block, VALUE method, VALUE module) {
      receiver_ = recv;
      method_ = method;
      block_ = block;
      module_ = module;
    }

    VALUE block() const {
      return block_;
    }

    void set_block(VALUE blk) {
      block_ = blk;
    }

    VALUE receiver() const {
      return receiver_;
    }

    VALUE method() const {
      return method_;
    }

    VALUE module() const {
      return module_;
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
    /** Arity of the method. @see Arity. */
    Fixnum* arity_;                                   // slot
    /** C file in which rb_define_method called. */
    String* file_;                                    // slot
    /** Name given at creation time. */
    Symbol* name_;                                    // slot
    /** Module on which created. */
    Module* module_;                                  // slot

    /** Function object that implements this method. */
    void* func_;

    int capi_lock_index_;

  public:   /* Accessors */

    /** Arity of the method within. @see Arity. */
    attr_accessor(arity, Fixnum);
    /** C file in which rb_define_method called. */
    attr_accessor(file, String);
    /** Name given at creation time. */
    attr_accessor(name, Symbol);
    /** Module on which created. */
    attr_accessor(module, Module);

    int capi_lock_index() const {
      return capi_lock_index_;
    }

  public:   /* Ruby bookkeeping */

    /** Statically held object type. */
    const static object_type type = NativeMethodType;

    /** Set class up in the VM. @see vm/ontology.cpp. */
    static void init(State* state);

    // Called when starting a new native thread, initializes any thread
    // local data.
    static void init_thread(State* state);

    // Called when a thread is exiting, to cleanup the thread local data.
    static void cleanup_thread(State* state);


  public:   /* Ctors */

    /**
     *  Create a NativeMethod object.
     *
     *  Takes a function to call in the form of a void*. +arity+ is used
     *  to figure out how to call the function properly.
     */
    static NativeMethod* create(State* state, String* file_name,
                                Module* module, Symbol* method_name,
                                void* func, Fixnum* arity,
                                int capi_lock_index);

  public:   /* Class Interface */

    /** Set up and call native method. */
    template <class ArgumentHandler>
    static Object* executor_implementation(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
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
