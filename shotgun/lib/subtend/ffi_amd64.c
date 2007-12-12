#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>

#include "shotgun.h"
#include "nmethod.h"
#include "symbol.h"
#include "object.h"
#include "nmc.h"
#include "ffi.h"
#include "library.h"
#include "string.h"
#include "hash.h"
#include "primitive_indexes.h"

#include "ffi_amd64.h"

/* Arguments passed in this order: %rdi, %rsi, %rdx, %rcx, %r8, %r9, stack */
static int ffi_amd64_reg_offset[] = {FFI_RDI, FFI_RSI, FFI_RDX, FFI_RCX, FFI_R8, FFI_R9}; 

/*
 * Generate a shim or wrapper around a C function.
 *
 * Produces an intermediary function directly into the
 * process at runtime using a hand-built machine code.
 * The creator provides the information about the 
 * function to call and the order and types of its
 * arguments (max 6) as well as its return type.
 *
 * The result is wrapped in an opaque Rubinius struct
 * so that it can be treated as an object.
 */ 
OBJECT ffi_amd64_generate_c_shim(STATE, 
                                 int arg_count, int* arg_types, 
                                 int ret_type, 
                                 void* func) 
{
  OBJECT  obj; 
  char*   start;
  char*   code; 
  void**  code_ptr; 
  void*   converter; 
  int     i, offset, reg_count, xmm_count; 
      
  /* Only 6 arguments currently */
  if(arg_count > 6) return Qnil;

  start = mmap(NULL, FFI_CODE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC,
               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  code = start;

  /* Function prolog */
  _B(0x55);                                 /* push %rbp */ 
  _B(0x48); _B(0x89); _B(0xe5);             /* mov %rsp, %rbp */
  _B(0x48); _B(0x83); _B(0xec); _B(96);     /* sub $96, %rsp */ 


  /* Byte offset from base pointer */
  offset = 0xfe; 

  /* These keep track of register positions needed */
  reg_count = 0; 
  xmm_count = 0; 

  /* Retrieve arguments from the Rubinius stack to our stack */
  for (i = 0 ; i < arg_count; ++i) {
    converter = ffi_get_to_converter(arg_types[i]);

    _B(0x48); _B(0xba); _Q(converter);      /* mov converter, %rdx */
    _B(0xff); _B(0xd2);                     /* callq *%rdx */ 

    switch (arg_types[i]) { 
      case FFI_TYPE_FLOAT:
        /* movss %xmm0, offset(%rbp) */
        offset -= 4; 
        _B(0xf3); _B(0x0f); _B(0x11); _B(0x45); _B(offset); 
        ++xmm_count; 
        break; 
        
      case FFI_TYPE_DOUBLE:
        /* movsd %xmm0, offset(%rbp) */
        offset -= 8; 
        _B(0xf2); _B(0x0f); _B(0x11); _B(0x45); _B(offset);
        ++xmm_count; 
        break; 

      default:
        /* push %rax */
        _B(0x50 + FFI_RAX);
        ++reg_count; 
        break; 
    }
  }

  /* Load arguments back from the stack to the appropriate registers */
  for (i = arg_count - 1; i >= 0; --i) {
    switch (arg_types[i]) { 
      case FFI_TYPE_FLOAT:
        /* movss offset(%rbp), %xmmN ; each %xmmN increment is + 8 */
        _B(0xf3); _B(0x0f); _B(0x10); _B(0x45 + (--xmm_count * 8)); _B(offset);
        offset += 4; 
        break; 
        
      case FFI_TYPE_DOUBLE:
        /* movsd offset(%rbp), %xmmN */
        _B(0xf2); _B(0x0f); _B(0x10); _B(0x45 + (--xmm_count * 8)); _B(offset);
        offset += 8; 
        break; 

      default:
        /* pop %<register> */
        _B(0x58 + ffi_amd64_reg_offset[--reg_count]); 
        break; 
    }
  }

  /* Call the given function */
  _B(0x48); _B(0xb8); _Q(func);           /* mov func, %rax ; %rdx may be taken */
  _B(0xff); _B(0xd0);                     /* callq *%rax */ 

  /* Set up return value processing */
  switch (ret_type) {
    case FFI_TYPE_VOID:
      _B(0x48); _B(0xbf); _Q(1);          /* mov $1, %rdi ; dummy */
      break; 

    /* Floats and doubles are already in %xmm0 */
    case FFI_TYPE_FLOAT:
    case FFI_TYPE_DOUBLE:
      break;

    default:
      _B(0x48); _B(0x89); _B(0xc7);       /* mov %rax, %rdi */
      break; 
  }

  converter = ffi_get_from_converter(ret_type); 

  /* Send return value back to rbx */
  _B(0x48); _B(0xba); _Q(converter);      /* mov converter, %rdx */
  _B(0xff); _B(0xd2);                     /* callq *%rdx */
  
  /* Clean up */
  _B(0xc9);                               /* leaveq */
  _B(0xc3);                               /* retq */

  NEW_STRUCT(obj, code_ptr, BASIC_CLASS(ffi_ptr), void*);
  *code_ptr = (void*)start;   /* Stash away the pointer */

  return obj;
}
