#ifndef RBX_BUILTIN_NATIVEMETHOD_HPP
#define RBX_BUILTIN_NATIVEMETHOD_HPP

/* Project */
#include "vm/executor.hpp"
#include "vm/vm.hpp"

#include "builtin/class.hpp"
#include "builtin/executable.hpp"
#include "builtin/memorypointer.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include "util/thread.hpp"
#include "gc_root.hpp"

#include "vm/object_utils.hpp"


namespace rubinius {

  /* Forwards */
  class Message;
  class NativeMethodFrame;

  /** A set of Handles. */
  typedef std::vector<TypedRoot<Object*>*> Handles;

  /** More prosaic name for Handles. */
  typedef intptr_t Handle;


  /**
   *  Thread-local info about native method calls.
   */
  class NativeMethodFraming {

  public:   /* Class Interface */

    /** Obtain the Framing for this thread. */
    static NativeMethodFraming* get();


  public:   /* Interface methods */

    /** Create or retrieve Handle for obj. */
    Handle get_handle(Object* obj);

    /** Create or retrieve Handle for a global object. */
    Handle get_handle_global(Object* obj);

    /** Obtain the Object the Handle represents. */
    Object* get_object(Handle hndl);

    /** Obtain the global Object the Handle represents*/
    Object* get_object_global(Handle hndl);

    /** Delete a global Object and its Handle. */
    void delete_global(Handle hndl);

    /** GC marking for Objects behind Handles. */
    void mark_handles(ObjectMark& mark);


  public:   /* Accessors */

    Object* block();

    void set_state(VM* vm) {
      state_ = vm;
    }

    VM* state() {
      return state_;
    }

    CallFrame* current_call_frame() {
      return current_call_frame_;
    }

    void set_current_call_frame(CallFrame* frame) {
      current_call_frame_ = frame;
    }

    NativeMethodFrame* current_native_frame() {
      return current_native_frame_;
    }

    void set_current_native_frame(NativeMethodFrame* frame) {
      current_native_frame_ = frame;
    }

    /** Set of Handles available in current Frame (convenience.) */
    Handles& handles();


  private:   /* Instance variables */

    /** VM in which executing. */
    VM*                 state_;
    /** Current callframe in Ruby-land. */
    CallFrame*          current_call_frame_;
    /** Current native callframe. */
    NativeMethodFrame*  current_native_frame_;
    /** Global object handles. */
    Handles             global_handles_;
  };


  /**
   *  Call frame for a native method.
   *
   *  @see NativeMethodFraming.
   */
  class NativeMethodFrame {

  public:
    NativeMethodFrame(NativeMethodFrame* prev)
      : previous_(prev)
    {}


  public:     /* Interface methods */

    /** Currently active/used Frame in this Framing i.e. thread. */
    NativeMethodFrame* current() {
      return NativeMethodFraming::get()->current_native_frame();
    }

    /** Create or retrieve a Handle for the Object. */
    Handle get_handle(VM*, Object* obj);

    /** Obtain the Object the Handle represents. */
    Object* get_object(Handle hndl);


  public:     /* Accessors */

    /** Handles to Objects used in this Frame. */
    Handles& handles() {
      return handles_;
    }

    /** Native Frame active before this call. */
    NativeMethodFrame* previous() {
      return previous_;
    }


  private:    /* Instance variables */

    /** Native Frame active before this call. @note This may NOT be the sender. --rue */
    NativeMethodFrame* previous_;

    /** Handles to Objects used in this Frame. */
    Handles handles_;
  };


  /**
   *  The various special method arities from a
   *  C method. If not one of these, then the
   *  number denotes the exact number of args
   *  in addition to the receiver instead.
   */
  enum Arity {
    INIT_FUNCTION = -99,
    ARGS_IN_RUBY_ARRAY = -3,
    RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY = -2,
    ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER = -1
  };


  /* The various function signatures needed since C++ requires strict typing here. */

  /** Generic function pointer used to store any type of functor. */
  typedef     void (*GenericFunctor)(void);

  /* Actual functor types. */

  typedef void   (*InitFunctor)     (void);   /**< The Init_<name> function. */

  typedef Handle (*ArgcFunctor)     (int, Handle*, Handle);
  typedef Handle (*OneArgFunctor)   (Handle);
  typedef Handle (*TwoArgFunctor)   (Handle, Handle);
  typedef Handle (*ThreeArgFunctor) (Handle, Handle, Handle);
  typedef Handle (*FourArgFunctor)  (Handle, Handle, Handle, Handle);
  typedef Handle (*FiveArgFunctor)  (Handle, Handle, Handle, Handle, Handle);
  typedef Handle (*SixArgFunctor)   (Handle, Handle, Handle, Handle, Handle, Handle);


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
   *  @todo   Add tests specifically for NativeMethod.
   *          Currently only in Subtend tests.
   */
  class NativeMethod : public Executable {
  public:   /* Ruby bookkeeping */

    /** Statically held object type. */
    const static object_type type = NativeMethodType;

    /** Set class up in the VM. @see vm/ontology.cpp. */
    static void register_class_with(VM* state);


  public:   /* Ctors */

    /**
     *  Create a NativeMethod object.
     *
     *  May take one of several types of functors but the calling
     *  code is responsible for figuring out which type to cast
     *  the functor back to for use, that information is not
     *  stored here.
     *
     *  @see  functor_as() for the cast back.
     */
    template <typename FunctorType>
      static NativeMethod* create(VM* state,
                                  String* file_name = as<String>(Qnil),
                                  Module* module = as<Module>(Qnil),
                                  Symbol* method_name = as<Symbol>(Qnil),
                                  FunctorType functor = static_cast<GenericFunctor>(NULL),
                                  Fixnum* arity = as<Fixnum>(Qnil))
      {
        NativeMethod* nmethod = state->new_object<NativeMethod>(G(nmethod));

        nmethod->arity(state, arity);
        nmethod->file_name(state, file_name);
        nmethod->method_name(state, method_name);
        nmethod->module(state, module);

        nmethod->functor(state, MemoryPointer::create(state, reinterpret_cast<void*>(functor)));

        nmethod->set_executor(&NativeMethod::executor_implementation);

        nmethod->primitive(state, state->symbol("nativemethod_call"));
        nmethod->serial(state, Fixnum::from(0));

        return nmethod;
      }

    /** Allocate a functional but empty NativeMethod. */
    static NativeMethod* allocate(VM* state);


  public:   /* Accessors */

    /** Arity of the method within. @see Arity. */
    attr_accessor(arity, Fixnum);
    /** C file in which rb_define_method called. */
    attr_accessor(file_name, String);
    /** Name given at creation time. */
    attr_accessor(method_name, Symbol);
    /** Module on which created. */
    attr_accessor(module, Module);
    /** Function object that implements this method. */
    attr_accessor(functor, MemoryPointer);


  public:   /* Class Interface */

    /** Set up and call native method. */
    static Object* executor_implementation(STATE, CallFrame* call_frame, Message& message);

    /**
     *  Attempt to load a C extension library and its main function.
     *
     *  The path should be the full path (relative or absolute) to the
     *  library, without the file extension. The name should be the
     *  entry point name, i.e. "Init_<extension>", where extension is
     *  the basename of the library. The function signature is:
     *
     *    void Init_<name>();
     *
     *  A NativeMethod for the function is returned, and can be executed
     *  to actually perform the loading.
     *
     *  Of possibly minor interest, the method's module is set to
     *  Rubinius. It should be updated accordingly when properly
     *  entered.
     */
    // Ruby.primitive :nativemethod_load_extension_entry_point
    static NativeMethod* load_extension_entry_point(STATE, String* path, String* name);


  public:   /* Instance methods */

    /** Call the C function. */
    Object* call(STATE, NativeMethodFrame* frame, Message& msg);


    /** Return the functor cast into the specified type. */
    template <typename FunctorType>
      FunctorType functor_as() const
      {
        return reinterpret_cast<FunctorType>(functor_->pointer);
      }


  private:  /* Slots */

    /** Arity of the method. @see Arity. */
    Fixnum* arity_;                                   // slot
    /** C file in which rb_define_method called. */
    String* file_name_;                               // slot
    /** Name given at creation time. */
    Symbol* method_name_;                             // slot
    /** Module on which created. */
    Module* module_;                                  // slot
    /** Function object that implements this method. */
    MemoryPointer* functor_;                          // slot


  public:   /* Type information */

    /** Type information for NativeMethod. */
    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };

  };    /* NativeMethod */

}


// Left here as an example of the stream interface. --rue
///**
// *  Define stream inserter for NMC frame info.
// *
// *  The only info coming from the context is really the name, file + line.
// */
//template<typename CharType, typename Traits>
//  std::basic_ostream<CharType, Traits>& operator<<(std::basic_ostream<CharType, Traits>& stream,
//                                                   const rubinius::NativeMethodContext* nmc) {
//  /* No futzing with broken streams. */
//  if (!stream.good()) {
//    return stream;
//  }
//
//  /* Ugh, sentries. */
//  typename std::basic_ostream<CharType, Traits>::sentry guard(stream);
//
//  if (guard) {
//    /* Using another stream here automates manipulators, yay. */
//    std::ostringstream out;
//
//    rubinius::Object* name = const_cast<rubinius::NativeMethodContext*>(nmc)->name();
//
//    out << rubinius::as<rubinius::Symbol>(name)->c_str(nmc->state())
//        << " in "
//        << nmc->current_file()
//        << ":"
//        << nmc->current_line()
//        << " (last known information.)";
//
//    /* Yes, we want the C string. */
//    stream << out.str().c_str();
//  }
//
//  return stream;
//}

#endif  /* NATIVEMETHOD_HPP */

