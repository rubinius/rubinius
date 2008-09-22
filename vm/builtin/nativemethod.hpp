#ifndef RBX_BUILTIN_NATIVEMETHOD_HPP
#define RBX_BUILTIN_NATIVEMETHOD_HPP

/* Project */
#include "builtin/class.hpp"
#include "builtin/executable.hpp"
#include "builtin/task.hpp"

#include "message.hpp"
#include "vm.hpp"

#include "vmnativemethod.hpp"
#include "builtin/nativemethodcontext.hpp"


namespace rubinius {

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
   *  TODO:   Add tests.
   */
  class NativeMethod : public Executable
  {
    public:   /* Ctors */

      template <typename FunctorType>
      static NativeMethod* create(FunctorType functor, int arity)
      {
        NativeMethod* obj = new NativeMethod();
        obj->my_functor = reinterpret_cast<GenericFunctor>(functor);
        obj->my_arity = Fixnum::from(arity);

        return obj;
      }

    public:   /* Interface */

             bool   execute(VM* state, Task* task, Message& message) {
                      return VMNativeMethod::execute(state, task, &message, this);
                    }

                    template <typename FunctorType>
      FunctorType   functor_as() const { return reinterpret_cast<FunctorType>(my_functor); }
                    /** Arity of the method. @see Arity. */
     const Fixnum*  arity() const { return my_arity; }

    public:   /* Ruby slots and bookkeeping */

    const static size_t fields = 5;
    const static object_type type = NativeMethodType;

  static void register_class_with(VM* state)
  {
    state->globals.nmethod.set(state->new_class("NativeMethod", state->globals.executable.get(), NativeMethod::fields));
    state->globals.nmethod.get()->set_object_type(state, NativeMethodType);
  }

    private:  /* Instance vars */

             Fixnum*  my_arity;         /**< Arity of the method. @see Arity */
     GenericFunctor   my_functor;       /**< Function object that implements this method. */

    public:

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };
  };

}

#endif  /* NATIVEMETHOD_HPP */

