#ifndef __RUBINIUS_INCLUDED__
#define __RUBINIUS_INCLUDED__ 1
#include <stdint.h>
#include "flags.h"

typedef struct rubinius_object* OBJECT;
typedef void * xpointer;

/* OOP layout:
 * [30 bits of data | 2 bits of tag]
 * if tag == 00, the whole thing is a pointer to a memory location.
 * if tag == 11, the data is a symbol index
 * if tag == 01, the data is a fixnum
 * if tag == 10, the data is a literal
 */

#define TAG_MASK    0x3
#define TAG_SHIFT   2

#define TAG_REF     0x0
#define TAG_DATA    0x3
#define TAG_FIXNUM  0x1
#define TAG_LITERAL 0x2

#define TAG(v) (((uintptr_t)v) & TAG_MASK)
#define APPLY_TAG(v, tag) ((OBJECT)(((uintptr_t)v << TAG_SHIFT) | tag))
#define STRIP_TAG(v) (((intptr_t)v) >> TAG_SHIFT)

#define DATA_P(v) (TAG(v) == TAG_DATA)
#define FIXNUM_P(v) (TAG(v) == TAG_FIXNUM)

#define DATA_MASK   0x7
#define DATA_SHIFT  3

#define DATA_TAG_SYMBOL 0x3
#define DATA_TAG_CUSTOM 0x7

#define DATA_TAG(v) ((uintptr_t)(v) & DATA_MASK)
#define DATA_APPLY_TAG(v, tag) (OBJECT)((v << DATA_SHIFT) | tag)
#define DATA_STRIP_TAG(v) (((uintptr_t)v) >> DATA_SHIFT)

#define SYMBOL_P(v) (DATA_TAG(v) == DATA_TAG_SYMBOL)
#define CUSTOM_P(v) (DATA_TAG(v) == DATA_TAG_CUSTOM)

/* How many bits of data are available in fixnum, not including
   the sign. */
#define FIXNUM_WIDTH 29

/* the sizeof(struct rubinius_object) must an increment of the platform 
   pointer size, so that the bytes located directly after a
   struct rubinius_object can hold a pointer which can be
   dereferenced. (an 32 bit platforms, pointers must be aligned
   on 32bit (word) boundaries. on 64 bit platforms, pointers probably
   have to be aligned on 64bit (double word) boundaries) */

/* On a 32 bit platform, I expect rubinius_object to take up
   1 + 1 + 2 + 4 + 4 = 12 bytes.
   on 64 bit platform,
   1 + 1 + 2 + 4 + 8 = 16 bytes.
*/

struct rubinius_object {
  union {
    struct {
      object_type     obj_type    : 3;
      gc_zone         gc_zone     : 2;
      unsigned int    copy_count  : 3;
      
      int Remember               : 1;
      int Marked                 : 1;
      int ForeverYoung           : 1;
      int CanStoreIvars          : 1;
      int StoresBytes            : 1;
      int RequiresCleanup        : 1;
      int IsBlockContext         : 1;
      int IsMeta                 : 1;
      
      int CTXFast                : 1;
      int IsTainted              : 1;
      int IsFrozen               : 1;
      int IsLittleEndian         : 1;
      int RefsAreWeak            : 1;
    };
    uint32_t forwarded_object;
  };
  uint32_t field_count;
  OBJECT klass;
  OBJECT field[];
};

/* 
A rubinius object can be followed by:
 - a series of fields, possibly including an ivar
 - a series of bytes (ByteArray)
 - a fast_context pointer
*/ 

#define CLASS_OBJECT(obj) (obj->klass)

#define NUM_FIELDS(obj)                 (obj->field_count)
#define SET_NUM_FIELDS(obj, fel)        (obj->field_count = fel)
#define SIZE_IN_BYTES_FIELDS(fel)       (sizeof(struct rubinius_object) + \
                                         fel*sizeof(OBJECT))
#define SIZE_IN_WORDS_FIELDS(fel)       (sizeof(struct rubinius_object)/sizeof(OBJECT) + fel)
#define SIZE_IN_BYTES(obj)              SIZE_IN_BYTES_FIELDS(obj->field_count)
#define SIZE_OF_BODY(obj)               (obj->field_count * sizeof(OBJECT))
#define ADDRESS_OF_FIELD(obj, fel)      (&obj->field[fel])
#define NTH_FIELD_DIRECT(obj, fel)      (obj->field[fel])
#define SET_FIELD_DIRECT(obj, fel, val) (obj->field[fel] = val)
#define BYTES_OF(obj)                   ((char*)obj->field)
#define FIXNUM_NEG(obj)                 ((intptr_t)obj < 0)

#ifdef Qfalse
#undef Qfalse
#undef Qtrue
#undef Qnil
#undef Qundef
#undef RTEST
#undef NIL_P
#undef SYMBOL_P
#undef FIXNUM_P
#endif

/* Standard Rubinius Representation

Bit layout of special literals:
             
 6:false    110   % 6 = 6
14:nil     1110   % 6 = 6
10:true    1010   % 6 = 2
18:undef  10010   % 6 = 2


false and nil share the same base bit pattern, allowing RTEST
to be a simple test for that bit pattern.
*/

#define Qfalse ((OBJECT)6L)
#define Qnil   ((OBJECT)14L)
#define Qtrue  ((OBJECT)10L)
#define Qundef ((OBJECT)18L)

#define FALSE_P(v) ((OBJECT)(v) == (OBJECT)Qfalse)
#define TRUE_P(v) ((OBJECT)(v) == (OBJECT)Qtrue)
#define NIL_P(v) ((OBJECT)(v) == (OBJECT)Qnil)
#define UNDEF_P(v) ((OBJECT)(v) == (OBJECT)Qundef)
#define RTEST(v) (((uintptr_t)(v) & 0x7) != 0x6)

#define REFERENCE_P(v) (TAG(v) == TAG_REF)

#define REFERENCE2_P(v) (v && REFERENCE_P(v))

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

static inline void object_copy_nongc_flags(OBJECT target, OBJECT source)
{
  target->obj_type        = source->obj_type;
  target->CanStoreIvars   = source->CanStoreIvars;
  target->StoresBytes     = source->StoresBytes;
  target->RequiresCleanup = source->RequiresCleanup;
  target->IsBlockContext  = source->IsBlockContext;
  target->IsMeta          = source->IsMeta;
}

#endif

