#ifndef RBS_FFI_X86_H
#define RBS_FFI_X86_H

OBJECT ffi_x86_generate_c_shim(STATE, int arg_count, int *arg_types,
                               int ret_type, void *func);

#endif
