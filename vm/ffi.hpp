#ifndef RBX_FFI_H
#define RBX_FFI_H

#define RBX_FFI_TYPE_OBJECT  0
#define RBX_FFI_TYPE_CHAR    1
#define RBX_FFI_TYPE_UCHAR   2
#define RBX_FFI_TYPE_SHORT   3
#define RBX_FFI_TYPE_USHORT  4
#define RBX_FFI_TYPE_INT     5
#define RBX_FFI_TYPE_UINT    6
#define RBX_FFI_TYPE_LONG    7
#define RBX_FFI_TYPE_ULONG   8
#define RBX_FFI_TYPE_LL      9
#define RBX_FFI_TYPE_ULL     10
#define RBX_FFI_TYPE_FLOAT   11
#define RBX_FFI_TYPE_DOUBLE  12
#define RBX_FFI_TYPE_PTR     13
#define RBX_FFI_TYPE_VOID    14
#define RBX_FFI_TYPE_STRING  15
#define RBX_FFI_TYPE_STATE   16
#define RBX_FFI_TYPE_STRPTR  17
#define RBX_FFI_TYPE_CHARARR 18

#include <ffi.h>
#include "prelude.hpp"
#include "object.hpp"
#include "message.hpp"

namespace rubinius {
  class MemoryPointer : public BuiltinType {
    public:
    const static size_t fields = 0;
    const static object_type type = MemPtrType;

    void* pointer;

    static MemoryPointer* create(STATE, void* ptr);

    OBJECT get_field(STATE, int offset, int type);
    void   set_field(STATE, int offset, int type, OBJECT val);
  };

  class NativeFunction : public Executable {
    public:
    struct ffi_stub {
      ffi_cif cif;
      size_t arg_count;
      int *arg_types;
      int ret_type;
      void *ep;
    };

    static const size_t fields = 7;
    OBJECT name;
    String* file;
    MemoryPointer* data;

    static void* find_symbol(STATE, OBJECT library, String* name);

    static size_t type_size(size_t type);
    static NativeFunction* create(STATE, OBJECT name, int args);
    void bind(STATE, int arg_count, int *arg_types, int ret_type, void* func);
    static NativeFunction* bind(STATE, String* library, String* name, Array* args, OBJECT ret);
    void **marshal_arguments(STATE, Message *msg);
    OBJECT call(STATE, Message* msg);
  };

}

#endif
