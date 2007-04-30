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
#define GC_MATURE_OBJECTS 0
#define GC_YOUNG_OBJECTS  1
#define GC_LARGE_OBJECTS  2

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

#ifndef INTERNAL_MACROS

#define SET_FIELD(obj, fel, val) rbs_set_field(obj, fel, val)

static inline OBJECT rbs_get_field(OBJECT in, int fel) {
  OBJECT obj;
  assert(fel < HEADER(in)->fields);
  obj = NTH_FIELD_DIRECT(in, fel);
  return obj;
}

#define NTH_FIELD(obj, fel) rbs_get_field(obj, fel)

static inline OBJECT rbs_set_field(OBJECT obj, int fel, OBJECT val) {
  assert(fel < HEADER(obj)->fields);
  OBJECT *slot = (OBJECT*)ADDRESS_OF_FIELD(obj, fel);
  assert(val != 12);
  *slot = val;
  return val;
}

#define SET_CLASS(obj, cls) (HEADER(obj)->klass = cls)

#endif

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


#define SYMBOL_P(v) (((OBJECT)(v) & ((1L<<SYMBOL_SHIFT)-1)) == SYMBOL_MARKER)
#define FIXNUM_P(v) (((OBJECT)(v) & ((1L<<FIXNUM_SHIFT)-1)) == FIXNUM_MARKER)

/* Standard Rubinius Representation */
#define Qfalse ((OBJECT)0L)
#define Qtrue  ((OBJECT)2L)
#define Qnil   ((OBJECT)4L)
#define Qundef ((OBJECT)6L)
#define RTEST(v) (((OBJECT)(v) & ~Qnil) != 0)
#define FALSE_P(v) ((OBJECT)(v) == Qfalse)
#define TRUE_P(v) ((OBJECT)(v) == Qtrue)
#define NIL_P(v) ((OBJECT)(v) == Qnil)
#define REFERENCE_P(v) ({ unsigned long _i = (unsigned long)v; _i > 10 && ((_i & 1L) == 0L); })
#define SYMBOL_MARKER 3
#define SYMBOL_SHIFT 2
#define FIXNUM_MARKER 1
#define FIXNUM_SHIFT 2

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

#ifndef STATE
#define STATE void* state
#endif

#ifndef __SHOTGUN__

OBJECT rbs_const_set(STATE, OBJECT module, char *name, OBJECT obj);
OBJECT rbs_const_get(STATE, OBJECT module, char *name);
OBJECT rbs_class_new(STATE, char *name, int fields, OBJECT obj);
char *rbs_symbol_to_cstring(STATE, OBJECT sym);
char *rbs_inspect(STATE, OBJECT obj);
OBJECT rbs_module_new(STATE, char *name, OBJECT ns);

#endif

#ifndef INTERNAL_MACROS
static inline long rbs_to_int(OBJECT obj) {
  int val = ((int)obj) >> FIXNUM_SHIFT;
  return val;
}

static inline OBJECT rbs_int_to_fixnum(int num) {
  OBJECT ret;
  ret = (num << FIXNUM_SHIFT) | FIXNUM_MARKER;
  
  return ret;
}


#define FIXNUM_TO_INT(obj) rbs_to_int(obj)
#define INT_TO_FIXNUM(int) rbs_int_to_fixnum(int)
#define I2N(i) INT_TO_FIXNUM(i)

#endif

#endif

