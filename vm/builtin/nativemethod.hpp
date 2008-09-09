#ifndef RBX_BUILTIN_NATIVEMETHOD_HPP
#define RBX_BUILTIN_NATIVEMETHOD_HPP

/* Project */
#include "builtin/executable.hpp"
#include "builtin/task.hpp"

#include "message.hpp"
#include "vm.hpp"

#include "vmnativemethod.hpp"


namespace rubinius {

  /**
   *  Ruby side of a NativeMethod, i.e. a C-implemented method.
   */
  class NativeMethod : public Executable
  {
    public:   /* Types */

      typedef void (*PointerTo)(void);


    public:   /* Ctors */

      static NativeMethod* create(PointerTo method)
      {
        NativeMethod* obj = new NativeMethod();
        obj->my_method = method;

        return obj;
      }

    public:   /* Interface */

      bool execute(VM* state, Task* task, Message& message)
      {
        return VMNativeMethod::execute(state, task, &message, this);
      }

      PointerTo actual_function_object() const { return my_method; }

    public:   /* Ruby slots and bookkeeping */

    private:  /* Instance vars */

      PointerTo  my_method;     /**< Function object that implements this method. */
  };

}

#endif  /* NATIVEMETHOD_HPP */

