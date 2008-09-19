#ifndef RBX_BUILTIN_NATIVEMETHOD_HPP
#define RBX_BUILTIN_NATIVEMETHOD_HPP

/* Project */
#include "executor.hpp"
#include "vm.hpp"

#include "builtin/class.hpp"
#include "builtin/executable.hpp"

#include "builtin/nativemethodcontext.hpp"


namespace rubinius {

  /* Forwards */
  class Message;
  class Task;
  class VM;


  /**
   *  The various special method arities from a
   *  C method. If not one of these, then the
   *  number denotes the exact number of args
   *  in addition to the receiver instead.
   */
  enum Arity {
    ARGS_IN_RUBY_ARRAY = -3,
    RECEIVER_PLUS_ARGS_IN_RUBY_ARRAY = -2,
    ARG_COUNT_ARGS_IN_C_ARRAY_PLUS_RECEIVER = -1
  };


  /* The various function signatures needed since C++ requires strict typing here. */

  /** Generic function pointer used to store any type of functor. */
  typedef     void (*GenericFunctor)(void);

  /* Actual functor types. */

  typedef HandleTo (*ArgcFunctor)     (int, HandleTo*, HandleTo);
  typedef HandleTo (*OneArgFunctor)   (HandleTo);
  typedef HandleTo (*TwoArgFunctor)   (HandleTo, HandleTo);
  typedef HandleTo (*ThreeArgFunctor) (HandleTo, HandleTo, HandleTo);
  typedef HandleTo (*FourArgFunctor)  (HandleTo, HandleTo, HandleTo, HandleTo);
  typedef HandleTo (*FiveArgFunctor)  (HandleTo, HandleTo, HandleTo, HandleTo, HandleTo);
  typedef HandleTo (*SixArgFunctor)   (HandleTo, HandleTo, HandleTo, HandleTo, HandleTo, HandleTo);


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
   *  TODO:   Add tests specifically for NativeMethod.
   *          Currently only in Subtend tests.
   */
  class NativeMethod : public Executable {
  public:   /* Ruby bookkeeping */

    /** Size in Object*'s. Executable + 2. */
    const static std::size_t fields = 5;
    /** Statically held object type. */
    const static object_type type = NMethodType;

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
      static NativeMethod* NativeMethod::create(VM* state,
                                                FunctorType functor = static_cast<GenericFunctor>(NULL),
                                                int arity = 0)
      {
        NativeMethod* nmethod = static_cast<NativeMethod*>(state->new_object(state->globals.nmethod.get()));

        nmethod->arity(state, Fixnum::from(arity));

        nmethod->my_functor = reinterpret_cast<GenericFunctor>(functor);

        nmethod->set_executor(&NativeMethod::executor_implementation);

        return nmethod;
      }

    /** Allocate a functional but empty NativeMethod. */
    static NativeMethod* allocate(VM* state);

  public:   /* Accessors */

    /** Arity of the method within. @see Arity. */
    attr_accessor(arity, Fixnum);


  public:   /* Interface */

    /**
     *  Handle C method call including its callstack.
     *
     *  Executor implementation.
     *
     *  In addition to setting up the NativeMethodContext for the call
     *  and the call itself (arguments, return value and all), we also
     *  handle setting up further calls from the method to other Ruby
     *  or C methods.
     *
     *  @note   Shamelessly tramples over the standard VMExecutable@execute.
     */
    static bool executor_implementation(VM* state, Executable* method, Task* task, Message& message);

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
      FunctorType NativeMethod::functor_as() const
      {
        return reinterpret_cast<FunctorType>(my_functor);
      }


  private:  /* Slots */

    /** Arity of the method. @see Arity. */
    Fixnum* arity_;           // slot


  private:  /* Instance variables */

    /** Function object that implements this method. */
    GenericFunctor my_functor;


  public:   /* Type information */

    /** Type information for NativeMethod. */
    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };

  };    /* NativeMethod */

}

#endif  /* NATIVEMETHOD_HPP */

