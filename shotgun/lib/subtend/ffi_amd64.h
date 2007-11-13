/* amd64 support for FFI */

#define FFI_CODE_SIZE 4092                

/* Macros for writing bytes, words, doublewords or quadwords,
 * assume the existence of a variable 'code' */
#define _B(byte)  (*code++ = (unsigned char) (byte))
#define _W(word)  {(*((unsigned short*)code) = (unsigned short) (word)); code += 4;}
#define _D(dword) {(*((unsigned int*)code) = (unsigned int) (dword)); code += 4;}
#define _Q(qword) {(*((unsigned long*)code) = (unsigned long) (qword)); code += 8;}

/* Register offsets, only 6 args in registers */
#define FFI_RAX 0
#define FFI_RDI 7
#define FFI_RSI 6
#define FFI_RDX 2
#define FFI_RCX 1
#define FFI_R8  8    
#define FFI_R9  9

/* Prototypes */
void ffi_amd64_flush(void* dest, int len); 
OBJECT ffi_amd64_generate_c_shim(STATE, 
                                 int arg_count, int* arg_types, 
                                 int ret_type, 
                                 void* func); 
