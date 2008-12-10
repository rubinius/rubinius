#ifndef RBX_BUILTIN_NATIVEFUNCTION_HPP
#define RBX_BUILTIN_NATIVEFUNCTION_HPP

#include "builtin/executable.hpp"

#include <ffi.h>

namespace rubinius {
  class NativeFunction : public Executable {
  public:
    static const object_type type = NativeFunctionType;

  private:
    Object* name_;         // slot
    Symbol* file_;         // slot
    MemoryPointer* data_; // slot
    Fixnum* required_;     // slot

  public:
    /* accessors */

    attr_accessor(name, Object);
    attr_accessor(file, Symbol);
    attr_accessor(data, MemoryPointer);
    attr_accessor(required, Fixnum);

    /* interface */

    static size_t type_size(size_t type);

    // Ruby.primitive :nativefunction_type_size
    static Fixnum* type_size_prim(STATE, Fixnum* type);

    static void init(STATE);

    static NativeFunction* create(STATE, Object* name, int args);
    static ExecuteStatus execute(STATE, Task* task, Message& msg);

    // Ruby.primitive :nativefunction_bind
    static NativeFunction* bind(STATE, Object* library, String* name, Array* args, Object* ret);

    void bind(STATE, int arg_count, int *arg_types, int ret_type, void* func);
    void **marshal_arguments(STATE, Message *msg);
    Object* call(STATE, Message* msg);

    struct ffi_stub {
      ffi_cif cif;
      size_t arg_count;
      int *arg_types;
      int ret_type;
      void *ep;
    };

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

}

#endif
