#include "vm.hpp"

#include "builtin/nativemethod.hpp"

namespace rubinius {

  class Task;
  class Message;


/* Class methods */


  void NativeMethod::register_class_with(VM* state)
  {
    state->globals.nmethod.set(state->new_class("NativeMethod", state->globals.executable.get(), NativeMethod::fields));
    state->globals.nmethod.get()->set_object_type(state, NMethodType);
  }

  template <typename FunctorType>
    NativeMethod* NativeMethod::create(VM* state, FunctorType functor, int arity)
    {
      NativeMethod* obj = static_cast<NativeMethod*>(state->new_object(state->globals.nmethod.get()));

      obj->arity(state, Fixnum::from(arity));

      obj->my_functor = reinterpret_cast<GenericFunctor>(functor);

      return obj;
    }

  NativeMethod* NativeMethod::allocate(VM* state)
  {
    return create<GenericFunctor>(state);
  }


/* Instance methods */


  bool NativeMethod::execute(VM* state, Task* task, Message& message)
  {
    return VMNativeMethod::execute(state, task, &message, this);
  }

  template <typename FunctorType>
  FunctorType NativeMethod::functor_as() const
  {
    return reinterpret_cast<FunctorType>(my_functor);
  }



}
