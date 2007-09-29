char *ffi_generate_c_stub(STATE, int args, void *func);
int ffi_type_size(int type);
void Init_ffi(STATE);
void* ffi_get_to_converter(int type);
void* ffi_get_from_converter(int type);
OBJECT ffi_function_create(STATE, OBJECT library, OBJECT name, OBJECT args, OBJECT ret);

OBJECT ffi_new_pointer(STATE, void *ptr);
#define ffi_pointer(ptr) (*DATA_STRUCT(ptr, void**))

#define FFI_TYPE_OBJECT  0
#define FFI_TYPE_CHAR    1
#define FFI_TYPE_UCHAR   2
#define FFI_TYPE_SHORT   3
#define FFI_TYPE_USHORT  4
#define FFI_TYPE_INT     5
#define FFI_TYPE_UINT    6
#define FFI_TYPE_LONG    7
#define FFI_TYPE_ULONG   8
#define FFI_TYPE_LL      9
#define FFI_TYPE_ULL     10
#define FFI_TYPE_FLOAT   11
#define FFI_TYPE_DOUBLE  12
#define FFI_TYPE_PTR     13
#define FFI_TYPE_VOID    14
#define FFI_TYPE_STRING  15
#define FFI_TYPE_STATE   16
#define FFI_TYPE_STRPTR  17
#define FFI_TYPE_CHARARR 18

typedef void (*nf_stub_ffi)(void);
typedef int (*nf_converter)();

#define NFUNC_FIELDS 7

#define nfunc_allocate(st) (object_memory_new_object_mature(st->om, st->global->ffi_func, NFUNC_FIELDS))
#define nfunc_get_data(obj) cmethod_get_bytecodes(obj)
#define nfunc_set_data(obj, data) cmethod_set_bytecodes(obj, data)
