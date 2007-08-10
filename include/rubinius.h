#ifndef __RUBINIUS_INCLUDED__
#define __RUBINIUS_INCLUDED__ 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uintptr_t OBJECT;

#define REFSIZE (sizeof(uintptr_t))

/* the sizeof(struct rubinius_object) must an increment of
   REFSIZE, so that the bytes located directly after a
   struct rubinius_object can hold a pointer which can be
   dereferenced. (an 32 bit platforms, pointers must be aligned
   on 32bit (word) boundaries. on 64 bit platforms, pointers probably
   have to be aligned on 64bit (double word) boundaries) */

/* On a 32 bit platform, I expect rubinius_object to take up
   1 + 1 + 2 + 4 + 4 + 4 + 4 = 20 bytes.
   on 64 bit platform,
   1 + 1 + 2 + 4 + 4 + 4 + 8 = 24 bytes.
*/

struct rubinius_object {
  uint8_t flags;
  uint8_t flags2;
  /* Used by the GC to store data about the object. */
  uint16_t gc;
  uint32_t object_id;
  uint32_t hash;
  
  uint32_t fields;
  OBJECT klass;
};

#include <assert.h>

#define HEADER(obj) ((struct rubinius_object*)obj)
#define OBJECTS(obj) ((OBJECT*)obj)
/*
#define FLAGS_OFFSET 0
#define FLAGS2_OFFSET 1
#define OBJID_OFFSET 4
#define CLASS_OFFSET 8
#define FIELDS_OFFSET 12
*/
/* Header size is in uintptr_t's */
//#define HEADER_SIZE 4
#define HEADER_SIZE (sizeof(struct rubinius_object) / REFSIZE)
/* Macros to find out the zone of an object. */
#define ZONE_BITS 2
#define ZONE_OFFSET 6
#define ZONE_MASK ((1 << ZONE_BITS) - 1)
#define GC_ZONE(obj) (HEADER(obj)->flags2 >> ZONE_OFFSET)
/* Mask the current flags2 to remove the current zone bits, then or
   them with the new desired ones. */
#define GC_ZONE_SET(obj, val) (HEADER(obj)->flags2 = ((HEADER(obj)->flags2 & ZONE_MASK) | (((val) & ZONE_MASK) << ZONE_OFFSET)))
#define GC_MATURE_OBJECTS 1
#define GC_YOUNG_OBJECTS  2
#define GC_LARGE_OBJECTS  3

#define GC_MAKE_FOREVER_YOUNG(obj) (HEADER(obj)->gc |= 0x8000)

#define CLASS_OBJECT(obj) (HEADER(obj)->klass)

#define NUM_FIELDS(obj) (HEADER(obj)->fields)
#define SET_NUM_FIELDS(obj, fel) (HEADER(obj)->fields = fel)
#define SIZE_IN_BYTES(obj) ((NUM_FIELDS(obj) + HEADER_SIZE) * REFSIZE)
#define SIZE_OF_BODY(obj) (NUM_FIELDS(obj) * REFSIZE)
#define ADDRESS_OF_FIELD(obj, fel) ((OBJECT)(OBJECTS(obj) + HEADER_SIZE + fel))
#define NTH_FIELD_DIRECT(obj, fel) (*(OBJECT*)(OBJECTS(obj) + HEADER_SIZE + fel))

#define BYTES_OF(obj) ((char*)(OBJECTS(obj) + HEADER_SIZE))
#define FIXNUM_NEG(obj) (((int)(obj)) < 0)

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
#define TAG_SYMBOL  0x3
#define TAG_FIXNUM  0x1
#define TAG_LITERAL 0x2

#define TAG(v) ((OBJECT)v & TAG_MASK)
#define APPLY_TAG(v, tag) ((v << TAG_SHIFT) | tag)
#define STRIP_TAG(v) (v >> TAG_SHIFT)

#define SYMBOL_SHIFT TAG_SHIFT
#define FIXNUM_SHIFT TAG_SHIFT

#define SYMBOL_P(v) (TAG(v) == TAG_SYMBOL)
#define FIXNUM_P(v) (TAG(v) == TAG_FIXNUM)

/* Standard Rubinius Representation */
#define Qfalse ((OBJECT)6L)
#define Qtrue  ((OBJECT)10L)
#define Qnil   ((OBJECT)14L)
#define Qundef ((OBJECT)18L)
#define FALSE_P(v) ((OBJECT)(v) == (OBJECT)Qfalse)
#define TRUE_P(v) ((OBJECT)(v) == (OBJECT)Qtrue)
#define NIL_P(v) ((OBJECT)(v) == (OBJECT)Qnil)
#define UNDEF_P(v) ((OBJECT)(v) == (OBJECT)Qundef)
#define RTEST(v) (!(FALSE_P(v) || NIL_P(v)))

#define REFERENCE_P(v) (TAG(v) == TAG_REF)

#define REFERENCE2_P(v) (v && REFERENCE_P(v))
#define FLAG_SET(obj, flag) (HEADER(obj)->flags |= flag)
#define FLAG_SET_P(obj, flag) ((HEADER(obj)->flags & flag) == flag)

#define FLAG_SET_ON(obj, fel, flag) (HEADER(obj)->fel |= flag)
#define FLAG_CLEAR_ON(obj, fel, flag) (HEADER(obj)->fel ^= flag)
#define FLAG_SET_ON_P(obj, fel, flag) ((HEADER(obj)->fel & flag) == flag)

#define FLAG2_SET(obj, flag) FLAG_SET_ON(obj, flags2, flag)
#define FLAG2_SET_P(obj, flag) FLAG_SET_ON_P(obj, flags2, flag)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif

