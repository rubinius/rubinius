#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

#include "shotgun.h"
#include "ffi.h"

#define FFI_CODE_SIZE 256

/* Macros for writing bytes or doublewords.
 * They assume the existence of a variable 'code'.
 */
#define _B(byte) \
  *code++ = (uint8_t) (byte)

#define _D(dword) \
  do { \
    uint32_t *tmp = (uint32_t *) code; \
    *tmp = (dword); \
    code += 4; \
  } while (0);

enum {
  EAX = 0,
  ECX,
  EDX,
  EBX,
  ESP,
  EBP,
  ESI,
  EDI
};

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
OBJECT ffi_x86_generate_c_shim(STATE, int arg_count, int *arg_types,
                               int ret_type, void *func)
{
  OBJECT obj;
  char *start, *code;
  void *converter, **code_ptr;
  uint32_t rel_addr;
  int i, offset, flags = MAP_PRIVATE;

  /* Only 6 arguments currently */
  if (arg_count > 6)
    return Qnil;

#ifdef MAP_ANONYMOUS
  flags |= MAP_ANONYMOUS;
#else
  flags |= MAP_ANON;
#endif

  code = start = mmap(NULL, FFI_CODE_SIZE,
                      PROT_READ | PROT_WRITE | PROT_EXEC,
                      flags, -1, 0);

  /* Function prolog */
  _B(0x50 + EBP);             /* push %ebp */
  _B(0x89); _B(0xe5);         /* movl %esp, %ebp */
  _B(0x83); _B(0xec); _B(96); /* subl $96, %esp */

  /* Byte offset from base pointer */
  offset = 0xfe;

  /* For each argument we're now calling ffi_to_TYPE().
   * Those functions will pop the argument off the Rubinius stack and
   * return them as a C value.
   * That return value will be on the top of the FPU stack if it's a
   * float or a double. If it's a 64 bit value, it will be in %eax and
   * %edx, or just in %eax otherwise.
   * We then store that converted value on our stack.
   */
  for (i = 0; i < arg_count; i++) {
    converter = ffi_get_to_converter(arg_types[i]);

    /* call converter */
    rel_addr = ((char *) converter) - code - 5;
    _B(0xe8); _D(rel_addr);

    switch (arg_types[i]) {
      case FFI_TYPE_FLOAT:
        /* fstp offset(%ebp) */
        offset -= 4;
        _B(0xd9); _B(0x5d); _B(offset);
        break;
      case FFI_TYPE_DOUBLE:
        /* fstpl offset(%ebp) */
        offset -= 8;
        _B(0xdd); _B(0x5d); _B(offset);
        break;
      case FFI_TYPE_LL:
      case FFI_TYPE_ULL:
        offset -= 4;
        _B(0x89); _B(0x55); _B(offset); /* movl %edx, offset(%ebp) */
        /* fall through */
      default:
        offset -= 4;
        _B(0x89); _B(0x45); _B(offset); /* movl %eax, offset(%ebp) */
        break;
    }
  }

  /* Load arguments from our stack and push them on the stack as the
   * given function's arguments.
   */
  for (i = arg_count - 1; i >= 0; i--) {
    /* As before, we only need to differentiate between 64 bit and
     * 32 bit values. No special handling for floating point numbers
     * is necessary, as these are just bytes on the stack now.
     */
    switch (ffi_type_size(arg_types[i])) {
      case 8:
        _B(0xff); _B(0x75); _B(offset + 4); /* push (offset+4)(%ebp) */
        _B(0xff); _B(0x75); _B(offset);     /* push (offset)(%ebp) */
        offset += 8;
        break;
      default:
        _B(0xff); _B(0x75); _B(offset);     /* push (offset)(%ebp) */
        offset += 4;
        break;
    }
  }

  /* call the given function */
  rel_addr = ((char *) func) - code - 5;
  _B(0xe8); _D(rel_addr);

  /* Set up return value processing */
  switch (ret_type) {
    case FFI_TYPE_VOID:
      _B(0x6a); _B(0x01); /* push $1 ; dummy */
      break;
    case FFI_TYPE_FLOAT:
      /* fstp offset(%ebp) */
      offset -= 4;
      _B(0xd9); _B(0x5d); _B(offset);

      _B(0xff); _B(0x75); _B(offset);     /* push offset(%ebp) */
      break;
    case FFI_TYPE_DOUBLE:
      /* fstpl offset(%ebp) */
      offset -= 8;
      _B(0xdd); _B(0x5d); _B(offset);

      _B(0xff); _B(0x75); _B(offset + 4); /* push (offset+4)(%ebp) */
      _B(0xff); _B(0x75); _B(offset);     /* push offset(%ebp) */
      break;
    case FFI_TYPE_LL:
    case FFI_TYPE_ULL:
      _B(0x50 + EDX); /* push %edx */
      /* fall through */
    default:
      _B(0x50 + EAX); /* push %eax */
      break;
  }

  converter = ffi_get_from_converter(ret_type);

  /* Send return value back to rbx */
  rel_addr = ((char *) converter) - code - 5;
  _B(0xe8); _D(rel_addr);

  /* Clean up */
  _B(0xc9); /* leave */
  _B(0xc3); /* ret */

  NEW_STRUCT(obj, code_ptr, BASIC_CLASS(ffi_ptr), void *);
  *code_ptr = start; /* Stash away the pointer */

  /* drop the writable flag */
  mprotect(start, FFI_CODE_SIZE, PROT_READ | PROT_EXEC);

  return obj;
}
