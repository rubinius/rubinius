#ifndef RBX_BUILTIN_NATIVEFUNCTION_HPP
#define RBX_BUILTIN_NATIVEFUNCTION_HPP

#include <ffi.h>

namespace rubinius {
  class NativeFunction : public Executable {
  public:
    static const size_t fields = 7;
    static const object_type type = NativeFuncType;

    OBJECT name; // slot
    SYMBOL file; // slot
    MemoryPointer* data; // slot
    FIXNUM required; // slot

    static void* find_symbol(STATE, OBJECT library, String* name);

    static size_t type_size(size_t type);
    static NativeFunction* create(STATE, OBJECT name, int args);
    void bind(STATE, int arg_count, int *arg_types, int ret_type, void* func);
    static NativeFunction* bind(STATE, String* library, String* name, Array* args, OBJECT ret);
    void **marshal_arguments(STATE, Message *msg);
    OBJECT call(STATE, Message* msg);

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
