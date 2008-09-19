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

  NativeMethod* NativeMethod::allocate(VM* state)
  {
    return create<GenericFunctor>(state);
  }


/* Instance methods */


  bool NativeMethod::execute(VM* state, Task* task, Message& message)
  {
    return VMNativeMethod::execute(state, task, &message, this);
  }



}
