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

#include "builtin/nativemethodcontext.hpp"


namespace rubinius {

  /* Forwards */
  class Message;
  class Task;


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

    /** Size in Object*'s. Executable + 2. */
    const static std::size_t fields = Executable::fields + 5;
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
        NativeMethod* nmethod = static_cast<NativeMethod*>(state->new_object(state->globals.nmethod.get()));

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


  public:   /* Interface */

    /**
     *  Handle C method call including its callstack.
     *
     *  In addition to setting up the NativeMethodContext for the call
     *  and the call itself (arguments, return value and all), we also
     *  handle setting up further calls from the method to other Ruby
     *  or C methods.
     *
     *  This method may be invoked by executor_implementation() or as
     *  a part of a return from a child context.
     *
     *  Sets the given context as the current.
     *
     *  @note   Shamelessly tramples over the standard VMExecutable@execute.
     */
    static ExecuteStatus activate_from(NativeMethodContext* context);

    /**
     *  Enter a new NativeMethod the first time.
     */
    static ExecuteStatus executor_implementation(VM* state, Task* task, Message& message);

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

    /**
     *  Call the C function.
     *
     *  We grab the information needed from the active context, convert
     *  everything necessary to handles, and then directly call the C
     *  function with those parameters. Eventually the return value is
     *  saved in the context and we jump back to the dispatch point.
     *
     *  (It is not possible to simply return since this method is in
     *  a different stack from the dispatcher.)
     */
    static void perform_call();

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

/**
 *  Define stream inserter for NMC frame info.
 *
 *  The only info coming from the context is really the name, file + line.
 */
template<typename CharType, typename Traits>
  std::basic_ostream<CharType, Traits>& operator<<(std::basic_ostream<CharType, Traits>& stream,
                                                   const rubinius::NativeMethodContext* nmc) {
  /* No futzing with broken streams. */
  if (!stream.good()) {
    return stream;
  }

  /* Ugh, sentries. */
  typename std::basic_ostream<CharType, Traits>::sentry guard(stream);

  if (guard) {
    /* Using another stream here automates manipulators, yay. */
    std::ostringstream out;

    rubinius::Object* name = const_cast<rubinius::NativeMethodContext*>(nmc)->name();

    out << rubinius::as<rubinius::Symbol>(name)->c_str(nmc->state())
        << " in "
        << nmc->current_file()
        << ":"
        << nmc->current_line()
        << " (last known information.)";

    /* Yes, we want the C string. */
    stream << out.str().c_str();
  }

  return stream;
}

#endif  /* NATIVEMETHOD_HPP */

