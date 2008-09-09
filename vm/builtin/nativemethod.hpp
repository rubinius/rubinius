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
   */
  class NativeMethod : public Executable
  {
    public:   /* Types */

      typedef HandleTo (*PointerTo)(HandleTo);


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

