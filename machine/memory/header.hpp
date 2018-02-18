#ifndef RBX_MEMORY_HEADER_HPP
#define RBX_MEMORY_HEADER_HPP

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <atomic>
#include <thread>

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "config.h"
#include "object_types.hpp"
#include "type_info.hpp"
#include "detection.hpp"
#include "util/thread.hpp"
#include "bug.hpp"

namespace rubinius {
  class Fixnum;

/* We use a variable length pointer tag system: The tag represents 1 to 3 bits
 * which uniquely identify a data type.
 *
 *   1 == rest is a fixnum
 *  00 == rest is an object reference
 * 010 == rest is a boolean literal
 * 110 == rest is a symbol
 *
 * NOTE: If these definitions change, the definitions for the C-API must be
 * updated in the configure script.
*/

#define TAG_REF          0x0L
#define TAG_REF_MASK     3
#define TAG_REF_WIDTH    2

#define TAG_FIXNUM       0x1L
#define TAG_FIXNUM_SHIFT 1
#define TAG_FIXNUM_MASK  1

#define TAG_BOOL         0x2L

#define TAG_SYMBOL       0x6L
#define TAG_SYMBOL_SHIFT 3
#define TAG_SYMBOL_MASK  7

#define APPLY_FIXNUM_TAG(v) ((Object*)(((intptr_t)(v) << TAG_FIXNUM_SHIFT) | TAG_FIXNUM))
#define STRIP_FIXNUM_TAG(v) (((intptr_t)(v)) >> TAG_FIXNUM_SHIFT)

#define APPLY_SYMBOL_TAG(v) ((Object*)(((intptr_t)(v) << TAG_SYMBOL_SHIFT) | TAG_SYMBOL))
#define STRIP_SYMBOL_TAG(v) (((intptr_t)(v)) >> TAG_SYMBOL_SHIFT)

/* Do not use these macros in code. They define the bit patterns for the
 * various object types and are used to define predicates. Use the predicates
 * (ie reference_p(), fixnum_p(), symbol_p()) directly.
 */
#define __REFERENCE_P__(v) (((intptr_t)(v) & TAG_REF_MASK) == TAG_REF)
#define __FIXNUM_P__(v)    (((intptr_t)(v) & TAG_FIXNUM_MASK) == TAG_FIXNUM)
#define __SYMBOL_P__(v)    (((intptr_t)(v) & TAG_SYMBOL_MASK) == TAG_SYMBOL)

/* How many bits of data are available in fixnum, not including the sign. */
#define FIXNUM_MAX_WIDTH  ((8 * sizeof(intptr_t)) - TAG_FIXNUM_SHIFT - 1)
#define FIXNUM_MIN_WIDTH  (FIXNUM_MAX_WIDTH + 1)

#define FIXNUM_MAX        (((intptr_t)1 << FIXNUM_MAX_WIDTH) - 1)
#define FIXNUM_MIN        (-1 - FIXNUM_MAX)

/* Standard Rubinius Representation
 *
 * Bit layout of special literals:
 *
 * 0x0a:false    1010   % 0xa = 0xa
 * 0x1a:nil     11010   % 0xa = 0xa
 * 0x12:true    10010   % 0xa = 0x2
 * 0x22:undef  100010   % 0xa = 0x2
 *
 *
 * false and nil share the same base bit pattern, allowing RTEST
 * to be a simple test for that bit pattern.
 */

/* NOTE ALSO! the special class array uses this bit pattern, so
 * if you change this, be sure to update the special class array! */
Object* const cFalse = reinterpret_cast<Object*>(0x0aL);
Object* const cNil   = reinterpret_cast<Object*>(0x1aL);
Object* const cTrue  = reinterpret_cast<Object*>(0x12L);
Object* const cUndef = reinterpret_cast<Object*>(0x22L);

// Indicates the mask to use to check if a value is ruby false.
// This mask matches both false and nil ONLY.
#define FALSE_MASK  0xf

// The bits that identify any nil value.
#define NIL_MASK    0x1f

#define CBOOL(v)                    (((uintptr_t)(v) & FALSE_MASK) != (uintptr_t)cFalse)
#define RBOOL(v)                    ((v) ? cTrue : cFalse)

// Some configuration flags

  /* rubinius_object gc zone, takes up two bits */
  typedef enum
  {
    UnspecifiedZone  = 0,
    MatureObjectZone = 1,
    YoungObjectZone  = 2,
    UnmanagedZone    = 3,
  } gc_zone;

  class Class;
  class Object;
  class VM;

  typedef std::vector<Object*> ObjectArray;

  enum LockStatus {
    eUnlocked,
    eLocked,
    eLockTimeout,
    eLockInterrupted,
    eLockError
  };

  typedef enum {
    eAuxWordEmpty    = 0,
    eAuxWordObjID    = 1,
    eAuxWordLock     = 2,
    eAuxWordHandle   = 3,
    eAuxWordInflated = 4
  } aux_meaning;

  const static int cAuxLockTIDShift = 8;
  const static int cAuxLockRecCountMask = 0xff;
  const static int cAuxLockRecCountMax  = 0xff - 1;

  const static bool cDebugThreading = false;

#define OBJECT_FLAGS_OBJ_TYPE        7
#define OBJECT_FLAGS_GC_ZONE         9
#define OBJECT_FLAGS_AGE            13
#define OBJECT_FLAGS_MEANING        16
#define OBJECT_FLAGS_FORWARDED      17
#define OBJECT_FLAGS_REMEMBER       18
#define OBJECT_FLAGS_MARKED         21
#define OBJECT_FLAGS_INIMMIX        22
#define OBJECT_FLAGS_PINNED         23
#define OBJECT_FLAGS_FROZEN         24
#define OBJECT_FLAGS_TAINTED        25
#define OBJECT_FLAGS_UNTRUSTED      26
#define OBJECT_FLAGS_LOCK_CONTENDED 27

  struct ObjectFlags {
    object_type  obj_type        : 8;
    gc_zone      zone            : 2;
    unsigned int age             : 7;
    aux_meaning  meaning         : 3;

    unsigned int Forwarded       : 1;
    unsigned int Remember        : 1;
    unsigned int Marked          : 3;

    unsigned int InImmix         : 1;
    unsigned int InLarge         : 1;
    unsigned int Pinned          : 1;

    unsigned int Frozen          : 1;
    unsigned int Tainted         : 1;
    unsigned int LockContended   : 1;

    unsigned int unused          : 1;
    uint32_t aux_word;
  };

  union HeaderWord {
    struct ObjectFlags f;
    uint64_t flags64;

    bool atomic_set(HeaderWord& old, HeaderWord& nw);
  };

  namespace capi {
    class Handle;
  }

  /* For clarity, using a struct of bitfields is desirable. However, there are
   * two critical downsides: 1. the C/C++ standards leave packing of struct
   * elements to the implementation of the C/C++ compiler; and 2. endianness
   * requires either mirroring the bitfield layout, or using endian-specific
   * constant values.
   *
   * The latter is a significant nuisance, while the former is fatal. We must
   * both be able to ensure that the bitfields fit into a defined amount of
   * memory and we need that to fit into a machine word so that
   * compare_exchange operations are lock-free.
   *
   * The following struct with bitfields is provided for clarity and assumes
   * little endian architecture. The macros that implement the header
   * operations use simple integral values and are the same regardless of
   * endianness (but obviously, the actual memory layout of these values would
   * depend on the endianness of the particular platform).
   *
   * This ordering of fields is rather arbitrary. They are grouped by function
   * to provide clarity.
   *
   *   typedef struct MemoryFlags {
   *     // Header status flags
   *     unsigned int extended         : 1;
   *     unsigned int forwarded        : 1;
   *
   *     // Memory flags
   *     unsigned int thread_id        : 12;
   *     unsigned int region           : 2;
   *     unsigned int pinned           : 1;
   *
   *     // Graph flags
   *     unsigned int visited          : 2;
   *
   *     // Garbage collector flags
   *     unsigned int marked           : 2;
   *     unsigned int scanned          : 1;
   *     unsigned int referenced       : 4;
   *
   *     // Data type flags
   *     unsigned int type_id          : 10;
   *     unsigned int data             : 1;
   *
   *     // Memory object flags
   *     unsigned int frozen           : 1;
   *     unsigned int tainted          : 1;
   *     unsigned int locked_count     : 3;
   *     unsigned int object_id        : 20;
   *     unsigned int type_specific    : 2;
   *   } MemoryFlags;
   */

#define RBX_MEMORY_HEADER_LOAD              (header.load())

#define RBX_MEMORY_UNSET_BIT(b)             (RBX_MEMORY_HEADER_LOAD \
                                              & ~(1L << RBX_MEMORY_ ## b ## _SHIFT))
#define RBX_MEMORY_SET_BIT(b)               (RBX_MEMORY_HEADER_LOAD \
                                              | (1L << RBX_MEMORY_ ## b ## _SHIFT))
#define RBX_MEMORY_GET_BIT(b)               (!!(RBX_MEMORY_HEADER_LOAD \
                                              & (1L << RBX_MEMORY_ ## b ## _SHIFT)))

#define RBX_MEMORY_SET_FIELD(b,v)           (RBX_MEMORY_HEADER_LOAD \
                                              | (((v & RBX_MEMORY_ ## b ## _VALUE_MASK) \
                                              << RBX_MEMORY_ ## b ## _SHIFT) \
                                              & RBX_MEMORY_ ## b ## _BIT_MASK))
#define RBX_MEMORY_GET_FIELD(b)             (((RBX_MEMORY_HEADER_LOAD \
                                              & RBX_MEMORY_ ## b ## _BIT_MASK) \
                                              >> RBX_MEMORY_ ## b ## _SHIFT) \
                                              & RBX_MEMORY_ ## b ## _VALUE_MASK)
#define RBX_MEMORY_BIT_MASK(b)              ((RBX_MEMORY_ ## b ## _VALUE_MASK) << \
                                              (RBX_MEMORY_ ## b ## _SHIFT))

#define RBX_MEMORY_MAX(b,f)                 (((f) & (RBX_MEMORY_ ## b ## _BIT_MASK)) \
                                              == (RBX_MEMORY_ ## b ## _BIT_MASK))

#define RBX_MEMORY_EXT_HEADER_SET(b,v)      \
  while(true) { \
    uintptr_t h = header.load(); \
    uintptr_t n = RBX_MEMORY_ ## b ## _SET(v); \
    if(header.compare_exchange_strong(h,n)) { \
      return; \
    } else { \
      std::this_thread::yield(); \
    } \
  }

#define RBX_MEMORY_EXT_HEADER_SET_BIT(b)    \
  while(true) { \
    uintptr_t h = header.load(); \
    uintptr_t n = RBX_MEMORY_ ## b ## _SET; \
    if(header.compare_exchange_strong(h,n)) { \
      return; \
    } else { \
      std::this_thread::yield(); \
    } \
  }

#define RBX_MEMORY_EXT_HEADER_UNSET_BIT(b)  \
  while(true) { \
    uintptr_t h = header.load(); \
    uintptr_t n = RBX_MEMORY_ ## b ## _UNSET; \
    if(header.compare_exchange_strong(h,n)) { \
      return; \
    } else { \
      std::this_thread::yield(); \
    } \
  }

#define RBX_MEMORY_HEADER_SET(b,v)          \
  while(true) { \
    uintptr_t h = header.load(); \
    uintptr_t n = RBX_MEMORY_ ## b ## _SET(v); \
    if(header.compare_exchange_strong(h,n)) { \
      return; \
    } else if(extended_p()) { \
      RBX_MEMORY_EXT_HEADER_ ## b ## _SET(v); \
      return; \
    } else { \
      std::this_thread::yield(); \
    } \
  }

#define RBX_MEMORY_HEADER_SET_BIT(b)        \
  while(true) { \
    uintptr_t h = header.load(); \
    uintptr_t n = RBX_MEMORY_ ## b ## _SET; \
    if(header.compare_exchange_strong(h,n)) { \
      return; \
    } else if(extended_p()) { \
      RBX_MEMORY_EXT_HEADER_ ## b ## _SET; \
      return; \
    } else { \
      std::this_thread::yield(); \
    } \
  }

#define RBX_MEMORY_HEADER_UNSET_BIT(b)      \
  while(true) { \
    uintptr_t h = header.load(); \
    uintptr_t n = RBX_MEMORY_ ## b ## _UNSET; \
    if(header.compare_exchange_strong(h,n)) { \
      return; \
    } else if(extended_p()) { \
      RBX_MEMORY_EXT_HEADER_ ## b ## _UNSET; \
      return; \
    } else { \
      std::this_thread::yield(); \
    } \
  }

#define RBX_MEMORY_HEADER_MASK                  0x7L
#define RBX_MEMORY_HEADER_PTR(f)                (f & ~RBX_MEMORY_HEADER_MASK)

#define RBX_MEMORY_EXT_HEADER_SIZE              (RBX_MEMORY_HEADER_LOAD \
                                                  & RBX_MEMORY_HEADER_MASK)

#define RBX_MEMORY_EXT_HEADER_WORD_MASK         0x3L
#define RBX_MEMORY_EXT_HEADER_WORD_TYPE_P(w,t)  (((w) & RBX_MEMORY_EXT_HEADER_WORD_MASK) == t)
#define RBX_MEMORY_EXT_HEADER_WORD_PTR(w)       ((w) & ~RBX_MEMORY_EXT_HEADER_WORD_MASK)

#define RBX_MEMORY_EXTENDED_SHIFT               0L

#define RBX_MEMORY_EXTENDED_P                   RBX_MEMORY_GET_BIT(EXTENDED)

#define RBX_MEMORY_HEADER_EXTENDED_SET(p)       (reinterpret_cast<uintptr_t>(p) | 1L)

#define RBX_MEMORY_FORWARDED_SHIFT              1L

#define RBX_MEMORY_FORWARDED_P                  RBX_MEMORY_GET_BIT(FORWARDED)
#define RBX_MEMORY_FORWARDED_SET                RBX_MEMORY_SET_BIT(FORWARDED)
#define RBX_MEMORY_FORWARDED_UNSET              RBX_MEMORY_UNSET_BIT(FORWARDED)

#define RBX_MEMORY_HEADER_FORWARDED_SET         RBX_MEMORY_HEADER_SET_BIT(FORWARDED)

#define RBX_MEMORY_THREAD_ID_SHIFT              2L
#define RBX_MEMORY_THREAD_ID_VALUE_MASK         ((1L << 12) - 1)
#define RBX_MEMORY_THREAD_ID_MAX                RBX_MEMORY_THREAD_ID_VALUE_MASK
#define RBX_MEMORY_THREAD_ID_BIT_MASK           RBX_MEMORY_BIT_MASK(THREAD_ID)

#define RBX_MEMORY_THREAD_ID_SET(v)             RBX_MEMORY_SET_FIELD(THREAD_ID,v)
#define RBX_MEMORY_THREAD_ID_GET                RBX_MEMORY_GET_FIELD(THREAD_ID)

#define RBX_MEMORY_HEADER_THREAD_ID_SET(v)      RBX_MEMORY_HEADER_SET(THREAD_ID,v)
#define RBX_MEMORY_EXT_HEADER_THREAD_ID_SET(v)  RBX_MEMORY_EXT_HEADER_SET(THREAD_ID,v)

#define RBX_MEMORY_MAX_THREAD_ID_P              RBX_MEMORY_MAX(THREAD_ID)

#define RBX_MEMORY_REGION_SHIFT                 14L
#define RBX_MEMORY_REGION_VALUE_MASK            ((1L << 2) - 1)
#define RBX_MEMORY_REGION_BIT_MASK              RBX_MEMORY_BIT_MASK(REGION)

#define RBX_MEMORY_REGION_SET(v)                RBX_MEMORY_SET_FIELD(REGION,v)
#define RBX_MEMORY_REGION_GET                   RBX_MEMORY_GET_FIELD(REGION)

#define RBX_MEMORY_HEADER_REGION_SET(v)         RBX_MEMORY_HEADER_SET(REGION,v)
#define RBX_MEMORY_EXT_HEADER_REGION_SET(v)     RBX_MEMORY_EXT_HEADER_SET(REGION,v)

#define RBX_MEMORY_PINNED_SHIFT                 16L

#define RBX_MEMORY_PINNED_P                     RBX_MEMORY_GET_BIT(PINNED)
#define RBX_MEMORY_PINNED_SET                   RBX_MEMORY_SET_BIT(PINNED)
#define RBX_MEMORY_PINNED_UNSET                 RBX_MEMORY_UNSET_BIT(PINNED)

#define RBX_MEMORY_HEADER_PINNED_SET            RBX_MEMORY_HEADER_SET_BIT(PINNED)
#define RBX_MEMORY_HEADER_PINNED_UNSET          RBX_MEMORY_HEADER_UNSET_BIT(PINNED)
#define RBX_MEMORY_EXT_HEADER_PINNED_SET        RBX_MEMORY_EXT_HEADER_SET_BIT(PINNED)
#define RBX_MEMORY_EXT_HEADER_PINNED_UNSET      RBX_MEMORY_EXT_HEADER_UNSET_BIT(PINNED)

#define RBX_MEMORY_VISITED_SHIFT                17L
#define RBX_MEMORY_VISITED_VALUE_MASK           ((1L << 2) - 1)
#define RBX_MEMORY_VISITED_BIT_MASK             RBX_MEMORY_BIT_MASK(VISITED)

#define RBX_MEMORY_VISITED_SET(v)               RBX_MEMORY_SET_FIELD(VISITED,v)
#define RBX_MEMORY_VISITED_GET                  RBX_MEMORY_GET_FIELD(VISITED)

#define RBX_MEMORY_HEADER_VISITED_SET(v)        RBX_MEMORY_HEADER_SET(VISITED,v)
#define RBX_MEMORY_EXT_HEADER_VISITED_SET(v)    RBX_MEMORY_EXT_HEADER_SET(VISITED,v)

#define RBX_MEMORY_MARKED_SHIFT                 19L
#define RBX_MEMORY_MARKED_VALUE_MASK            ((1L << 2) - 1)
#define RBX_MEMORY_MARKED_BIT_MASK              RBX_MEMORY_BIT_MASK(MARKED)

#define RBX_MEMORY_MARKED_SET(v)                RBX_MEMORY_SET_FIELD(MARKED,v)
#define RBX_MEMORY_MARKED_GET                   RBX_MEMORY_GET_FIELD(MARKED)

#define RBX_MEMORY_HEADER_MARKED_SET(v)         RBX_MEMORY_HEADER_SET(MARKED,v)
#define RBX_MEMORY_EXT_HEADER_MARKED_SET(v)     RBX_MEMORY_EXT_HEADER_SET(MARKED,v)

#define RBX_MEMORY_SCANNED_SHIFT                21L

#define RBX_MEMORY_SCANNED_P                    RBX_MEMORY_GET_BIT(SCANNED)
#define RBX_MEMORY_SCANNED_SET                  RBX_MEMORY_SET_BIT(SCANNED)
#define RBX_MEMORY_SCANNED_UNSET                RBX_MEMORY_UNSET_BIT(SCANNED)

#define RBX_MEMORY_HEADER_SCANNED_SET           RBX_MEMORY_HEADER_SET_BIT(SCANNED)
#define RBX_MEMORY_HEADER_SCANNED_UNSET         RBX_MEMORY_HEADER_UNSET_BIT(SCANNED)
#define RBX_MEMORY_EXT_HEADER_SCANNED_SET       RBX_MEMORY_EXT_HEADER_SET_BIT(SCANNED)
#define RBX_MEMORY_EXT_HEADER_SCANNED_UNSET     RBX_MEMORY_EXT_HEADER_UNSET_BIT(SCANNED)

#define RBX_MEMORY_REFERENCED_SHIFT             22L
#define RBX_MEMORY_REFERENCED_VALUE_MASK        ((1L << 4) - 1)
#define RBX_MEMORY_REFERENCED_BIT_MASK          RBX_MEMORY_BIT_MASK(REFERENCED)

#define RBX_MEMORY_REFERENCED_SET(v)            RBX_MEMORY_SET_FIELD(REFERENCED,v)
#define RBX_MEMORY_REFERENCED_GET               RBX_MEMORY_GET_FIELD(REFERENCED)

#define RBX_MEMORY_HEADER_REFERENCED_SET(v)     RBX_MEMORY_HEADER_SET(REFERENCED,v)
#define RBX_MEMORY_EXT_HEADER_REFERENCED_SET(v) RBX_MEMORY_EXT_HEADER_SET(REFERENCED,v)

#define RBX_MEMORY_TYPE_ID_SHIFT                26L
#define RBX_MEMORY_TYPE_ID_VALUE_MASK           ((1L << 10) - 1)
#define RBX_MEMORY_TYPE_ID_MAX                  RBX_MEMORY_TYPE_ID_VALUE_MASK
#define RBX_MEMORY_TYPE_ID_BIT_MASK             RBX_MEMORY_BIT_MASK(TYPE_ID)

#define RBX_MEMORY_TYPE_ID_SET(v)               RBX_MEMORY_SET_FIELD(TYPE_ID,v)
#define RBX_MEMORY_TYPE_ID_GET                  RBX_MEMORY_GET_FIELD(TYPE_ID)

#define RBX_MEMORY_HEADER_TYPE_ID_SET(v)        RBX_MEMORY_HEADER_SET(TYPE_ID,v)
#define RBX_MEMORY_EXT_HEADER_TYPE_ID_SET(v)    RBX_MEMORY_EXT_HEADER_SET(TYPE_ID,v)

#define RBX_MEMORY_DATA_SHIFT                   36L
#define RBX_MEMORY_DATA_VALUE_MASK              1L
#define RBX_MEMORY_DATA_BIT_MASK                RBX_MEMORY_BIT_MASK(DATA)

#define RBX_MEMORY_DATA_P                       RBX_MEMORY_GET_BIT(DATA)
#define RBX_MEMORY_DATA_SET(v)                  RBX_MEMORY_SET_FIELD(DATA,v)

#define RBX_MEMORY_FROZEN_SHIFT                 37L

#define RBX_MEMORY_FROZEN_P                     RBX_MEMORY_GET_BIT(FROZEN)
#define RBX_MEMORY_FROZEN_SET                   RBX_MEMORY_SET_BIT(FROZEN)
#define RBX_MEMORY_FROZEN_UNSET                 RBX_MEMORY_UNSET_BIT(FROZEN)

#define RBX_MEMORY_HEADER_FROZEN_SET            RBX_MEMORY_HEADER_SET_BIT(FROZEN)
#define RBX_MEMORY_HEADER_FROZEN_UNSET          RBX_MEMORY_HEADER_UNSET_BIT(FROZEN)
#define RBX_MEMORY_EXT_HEADER_FROZEN_SET        RBX_MEMORY_EXT_HEADER_SET_BIT(FROZEN)
#define RBX_MEMORY_EXT_HEADER_FROZEN_UNSET      RBX_MEMORY_EXT_HEADER_UNSET_BIT(FROZEN)

#define RBX_MEMORY_TAINTED_SHIFT                38L

#define RBX_MEMORY_TAINTED_P                    RBX_MEMORY_GET_BIT(TAINTED)
#define RBX_MEMORY_TAINTED_SET                  RBX_MEMORY_SET_BIT(TAINTED)
#define RBX_MEMORY_TAINTED_UNSET                RBX_MEMORY_UNSET_BIT(TAINTED)

#define RBX_MEMORY_HEADER_TAINTED_SET           RBX_MEMORY_HEADER_SET_BIT(TAINTED)
#define RBX_MEMORY_HEADER_TAINTED_UNSET         RBX_MEMORY_HEADER_UNSET_BIT(TAINTED)
#define RBX_MEMORY_EXT_HEADER_TAINTED_SET       RBX_MEMORY_EXT_HEADER_SET_BIT(TAINTED)
#define RBX_MEMORY_EXT_HEADER_TAINTED_UNSET     RBX_MEMORY_EXT_HEADER_UNSET_BIT(TAINTED)

#define RBX_MEMORY_LOCKED_COUNT_SHIFT           39L
#define RBX_MEMORY_LOCKED_COUNT_VALUE_MASK      ((1L << 3) - 1)
#define RBX_MEMORY_LOCKED_COUNT_MAX             RBX_MEMORY_LOCKED_COUNT_VALUE_MASK
#define RBX_MEMORY_LOCKED_COUNT_BIT_MASK        RBX_MEMORY_BIT_MASK(LOCKED_COUNT)

#define RBX_MEMORY_LOCKED_COUNT_SET(v)          RBX_MEMORY_SET_FIELD(LOCKED_COUNT,v)
#define RBX_MEMORY_LOCKED_COUNT_GET             RBX_MEMORY_GET_FIELD(LOCKED_COUNT)

#define RBX_MEMORY_MAX_LOCKED_COUNT_P           RBX_MEMORY_MAX(LOCKED_COUNT)

#define RBX_MEMORY_HEADER_LOCKED_COUNT_SET(v)      RBX_MEMORY_HEADER_SET(LOCKED_COUNT,v)
#define RBX_MEMORY_EXT_HEADER_LOCKED_COUNT_SET(v)  RBX_MEMORY_EXT_HEADER_SET(LOCKED_COUNT,v)

#define RBX_MEMORY_OBJECT_ID_SHIFT              42L
#define RBX_MEMORY_OBJECT_ID_VALUE_MASK         ((1L << 20) - 1)
#define RBX_MEMORY_OBJECT_ID_MAX                RBX_MEMORY_OBJECT_ID_VALUE_MASK
#define RBX_MEMORY_OBJECT_ID_BIT_MASK           RBX_MEMORY_BIT_MASK(OBJECT_ID)

#define RBX_MEMORY_OBJECT_ID_SET(v)             RBX_MEMORY_SET_FIELD(OBJECT_ID,v)
#define RBX_MEMORY_OBJECT_ID_GET                RBX_MEMORY_GET_FIELD(OBJECT_ID)

#define RBX_MEMORY_HEADER_OBJECT_ID_SET(v)      RBX_MEMORY_HEADER_SET(OBJECT_ID,v)
#define RBX_MEMORY_EXT_HEADER_OBJECT_ID_SET(v)  RBX_MEMORY_EXT_HEADER_SET(OBJECT_ID,v)

#define RBX_MEMORY_TYPE_SPECIFIC_SHIFT          62L
#define RBX_MEMORY_TYPE_SPECIFIC_VALUE_MASK     ((1L << 2) - 1)
#define RBX_MEMORY_TYPE_SPECIFIC_BIT_MASK       RBX_MEMORY_BIT_MASK(TYPE_SPECIFIC)

#define RBX_MEMORY_TYPE_SPECIFIC_SET(v)         RBX_MEMORY_SET_FIELD(TYPE_SPECIFIC,v)
#define RBX_MEMORY_TYPE_SPECIFIC_GET            RBX_MEMORY_GET_FIELD(TYPE_SPECIFIC)

#define RBX_MEMORY_HEADER_TYPE_SPECIFIC_SET(v)      RBX_MEMORY_HEADER_SET(TYPE_SPECIFIC,v)
#define RBX_MEMORY_EXT_HEADER_TYPE_SPECIFIC_SET(v)  RBX_MEMORY_EXT_HEADER_SET(TYPE_SPECIFIC,v)

  // A machine word so that compare_exchange is lock-free. We assume 64-bit;
  typedef uintptr_t MemoryFlags;

  typedef uintptr_t ExtendedHeaderWord;

  enum MemoryRegion {
    eThreadRegion = 0,
    eFirstRegion,
    eSecondRegion,
    eLargeRegion,
  };

  struct MemoryHandle {
  };

  struct ExtendedHeader {
    std::atomic<MemoryFlags> header;
    ExtendedHeaderWord words[0];

    enum WordType {
      eObjectID = 0,
      eHandle,
      eRefCount,
      eLock,
    };

    ExtendedHeader(const MemoryFlags h)
      : header(h)
    {
      words[0] = 0;
    }

    ExtendedHeader(const MemoryFlags h, const ExtendedHeader* eh)
      : header(h)
    {
      for(int i = 0; i < eh->size(); i++) {
        words[i] = eh->words[i];
      }

      words[eh->size()] = 0;
    }

    ~ExtendedHeader() {
      for(int i = 0; i < size(); i++) {
        if(RBX_MEMORY_EXT_HEADER_WORD_TYPE_P(words[i], eHandle)) {
          delete reinterpret_cast<MemoryHandle*>(RBX_MEMORY_EXT_HEADER_WORD_PTR(words[i]));
        }
      }
    }

    static ExtendedHeader* create(const MemoryFlags h) {
      uintptr_t* mem = new uintptr_t[2];

      return new(mem) ExtendedHeader(h);
    }

    static ExtendedHeader* create(const MemoryFlags h, const ExtendedHeader* eh) {
      uintptr_t* mem = new uintptr_t[eh->size() + 2];

      return new(mem) ExtendedHeader(h);
    }

    static ExtendedHeader* create_object_id(const MemoryFlags h) {
      return create(h);
    }

    static ExtendedHeader* create_object_id(const MemoryFlags h, const ExtendedHeader* eh) {
      return create(h, eh);
    }

    static ExtendedHeader* create_handle(const MemoryFlags h) {
      return create(h);
    }

    static ExtendedHeader* create_handle(const MemoryFlags h, const ExtendedHeader* eh) {
      return create(h, eh);
    }

    static ExtendedHeader* create_reference(const MemoryFlags h) {
      return create(h);
    }

    static ExtendedHeader* create_reference(const MemoryFlags h, const ExtendedHeader* eh) {
      return create(h, eh);
    }

    static ExtendedHeader* create_lock(const MemoryFlags h) {
      return create(h);
    }

    static ExtendedHeader* create_lock(const MemoryFlags h, const ExtendedHeader* eh) {
      return create(h, eh);
    }

    void delete_header() {
      this->~ExtendedHeader();
      delete[] reinterpret_cast<uintptr_t*>(this);
    }

    int size() const {
      return RBX_MEMORY_EXT_HEADER_SIZE;
    }

    MemoryHandle* get_handle() const {
      for(int i = 0; i < size(); i++) {
        if(RBX_MEMORY_EXT_HEADER_WORD_TYPE_P(words[i], eHandle)) {
          return reinterpret_cast<MemoryHandle*>(RBX_MEMORY_EXT_HEADER_WORD_PTR(words[i]));
        }
      }

      return nullptr;
    }

    unsigned int thread_id() {
      return RBX_MEMORY_THREAD_ID_GET;
    }

    void set_thread_id(uintptr_t id) {
      RBX_MEMORY_EXT_HEADER_THREAD_ID_SET(id);
    }

    MemoryRegion region() const {
      return static_cast<MemoryRegion>(RBX_MEMORY_REGION_GET);
    }

    void region(MemoryRegion region) {
      RBX_MEMORY_EXT_HEADER_REGION_SET(region);
    }

    bool pinned_p() const {
      return RBX_MEMORY_PINNED_P;
    }

    void set_pinned() {
      RBX_MEMORY_EXT_HEADER_PINNED_SET;
    }

    void unset_pinned() {
      RBX_MEMORY_EXT_HEADER_PINNED_UNSET;
    }

    bool visited_p(int flag) const {
      return RBX_MEMORY_VISITED_GET == flag;
    }

    void set_visited(int flag) {
      RBX_MEMORY_EXT_HEADER_VISITED_SET(flag);
    }

    bool marked_p(unsigned int mark) const {
      return RBX_MEMORY_MARKED_GET == mark;
    }

    void set_marked(unsigned int mark) {
      RBX_MEMORY_EXT_HEADER_MARKED_SET(mark)
    }

    bool scanned_p() const {
      return RBX_MEMORY_SCANNED_P;
    }

    void set_scanned() {
      RBX_MEMORY_EXT_HEADER_SCANNED_SET;
    }

    void unset_scanned() {
      RBX_MEMORY_EXT_HEADER_SCANNED_UNSET;
    }

    unsigned int referenced() const {
      return RBX_MEMORY_REFERENCED_GET;
    }

    unsigned int add_referenced() {
      return 0;
    }

    object_type type_id() const {
      return static_cast<object_type>(RBX_MEMORY_TYPE_ID_GET);
    }

    bool data_p() const {
      return RBX_MEMORY_DATA_P;
    }

    bool object_p() const {
      return !RBX_MEMORY_DATA_P;
    }

    bool frozen_p() const {
      return RBX_MEMORY_FROZEN_P;
    }

    void set_frozen() {
      RBX_MEMORY_EXT_HEADER_FROZEN_SET;
    }

    void unset_frozen() {
      RBX_MEMORY_EXT_HEADER_FROZEN_UNSET;
    }

    bool tainted_p() const {
      return RBX_MEMORY_TAINTED_P;
    }

    void set_tainted() {
      RBX_MEMORY_EXT_HEADER_TAINTED_SET;
    }

    void unset_tainted() {
      RBX_MEMORY_EXT_HEADER_TAINTED_UNSET;
    }

    unsigned int locked_count() const {
      return RBX_MEMORY_LOCKED_COUNT_GET;
    }

    uintptr_t object_id() const {
      uintptr_t id = RBX_MEMORY_OBJECT_ID_GET;

      if(id < RBX_MEMORY_OBJECT_ID_MAX) return id;

      for(int i = 0; i < size(); i++) {
        if(RBX_MEMORY_EXT_HEADER_WORD_TYPE_P(words[i], eObjectID)) {
          return reinterpret_cast<uintptr_t>(RBX_MEMORY_EXT_HEADER_WORD_PTR(words[i]));
        }
      }

      return 0;
    }

    bool set_object_id(uintptr_t id) {
      while(true) {
        MemoryFlags h = header;
        MemoryFlags nh = RBX_MEMORY_OBJECT_ID_SET(id);

        if(header.compare_exchange_strong(h, nh)) {
          return true;
        } else {
          if(object_id() != 0) return false;
        }
      }
    }

    unsigned int type_specific() const {
      return RBX_MEMORY_TYPE_SPECIFIC_GET;
    }

    void type_specific(unsigned int value) {
      RBX_MEMORY_EXT_HEADER_TYPE_SPECIFIC_SET(value);
    }
  };

  struct MemoryHeader {
    std::atomic<MemoryFlags> header;

    static std::atomic<uintptr_t> object_id_counter;

    static void bootstrap(STATE);

    void initialize(int thread_id, MemoryRegion region, object_type type, bool data) {
      header = 0;

      RBX_MEMORY_THREAD_ID_SET(thread_id);
      RBX_MEMORY_REGION_SET(region);
      RBX_MEMORY_TYPE_ID_SET(type);
      RBX_MEMORY_DATA_SET(data);
    }

    static void initialize(MemoryHeader* obj,
        int thread_id, MemoryRegion region, object_type type, bool data)
    {
      obj->initialize(thread_id, region, type, data);
    }

    ExtendedHeader* extended_header() const {
      return const_cast<ExtendedHeader*>(
          reinterpret_cast<const ExtendedHeader*>(
            RBX_MEMORY_HEADER_PTR(header.load())));
    }

    unsigned int max_thread_id() const {
      return RBX_MEMORY_THREAD_ID_MAX;
    }

    unsigned int max_type_id() const {
      return RBX_MEMORY_TYPE_ID_MAX;
    }

    unsigned int max_object_id() const {
      return RBX_MEMORY_OBJECT_ID_MAX;
    }

    unsigned int max_locked_count() const {
      return RBX_MEMORY_LOCKED_COUNT_MAX;
    }

    bool extended_p() const {
      return RBX_MEMORY_EXTENDED_P;
    }

    bool forwarded_p() const {
      return RBX_MEMORY_FORWARDED_P;
    }

    int thread_id() const {
      if(extended_p()) return extended_header()->thread_id();

      return RBX_MEMORY_THREAD_ID_GET;
    }

    void set_thread_id(uintptr_t id) {
      if(extended_p()) {
        extended_header()->set_thread_id(id);
      } else {
        RBX_MEMORY_HEADER_THREAD_ID_SET(id);
      }
    }

    MemoryRegion region() const {
      if(extended_p()) return extended_header()->region();

      return static_cast<MemoryRegion>(RBX_MEMORY_REGION_GET);
    }

    void region(MemoryRegion region) {
      if(extended_p()) {
        extended_header()->region(region);
      } else {
        RBX_MEMORY_HEADER_REGION_SET(region);
      }
    }

    bool pinned_p() const {
      if(extended_p()) return extended_header()->pinned_p();

      return RBX_MEMORY_PINNED_P;
    }

    void set_pinned() {
      if(extended_p()) {
        extended_header()->set_pinned();
      } else {
        RBX_MEMORY_HEADER_PINNED_SET;
      }
    }

    void unset_pinned() {
      if(extended_p()) {
        extended_header()->unset_pinned();
      } else {
        RBX_MEMORY_HEADER_PINNED_UNSET;
      }
    }

    bool visited_p(int flag) const {
      if(extended_p()) return extended_header()->visited_p(flag);

      return RBX_MEMORY_VISITED_GET == flag;
    }

    void set_visited(int flag) {
      if(extended_p()) {
        extended_header()->set_visited(flag);
      } else {
        RBX_MEMORY_HEADER_VISITED_SET(flag);
      }
    }

    bool marked_p(unsigned int mark) const {
      if(extended_p()) return extended_header()->marked_p(mark);

      return RBX_MEMORY_MARKED_GET == mark;
    }

    void set_marked(unsigned int mark) {
      if(extended_p()) {
        extended_header()->set_marked(mark);
      } else {
        RBX_MEMORY_HEADER_MARKED_SET(mark)
      }
    }

    bool scanned_p() const {
      if(extended_p()) return extended_header()->scanned_p();

      return RBX_MEMORY_SCANNED_P;
    }

    void set_scanned() {
      if(extended_p()) {
        extended_header()->set_scanned();
      } else {
        RBX_MEMORY_HEADER_SCANNED_SET;
      }
    }

    void unset_scanned() {
      if(extended_p()) {
        extended_header()->unset_scanned();
      } else {
        RBX_MEMORY_HEADER_SCANNED_UNSET;
      }
    }

    unsigned int referenced() const {
      if(extended_p()) return extended_header()->referenced();

      return RBX_MEMORY_REFERENCED_GET;
    }

    void add_referenced() {
      if(extended_p()) {
        extended_header()->add_referenced();
      } else {
        // TODO: refcount
      }
    }

    object_type type_id() const {
      if(extended_p()) return extended_header()->type_id();

      return static_cast<object_type>(RBX_MEMORY_TYPE_ID_GET);
    }

    bool data_p() const {
      if(extended_p()) return extended_header()->data_p();

      return RBX_MEMORY_DATA_P;
    }

    bool object_p() const {
      if(extended_p()) return extended_header()->object_p();

      return !RBX_MEMORY_DATA_P;
    }

    bool reference_p() const {
      return __REFERENCE_P__(this);
    }

    bool frozen_p() const {
      if(extended_p()) return extended_header()->frozen_p();

      return RBX_MEMORY_FROZEN_P;
    }

    void set_frozen() {
      if(extended_p()) {
        extended_header()->set_frozen();
      } else {
        RBX_MEMORY_HEADER_FROZEN_SET;
      }
    }

    void unset_frozen() {
      if(extended_p()) {
        extended_header()->unset_frozen();
      } else {
        RBX_MEMORY_HEADER_FROZEN_UNSET;
      }
    }

    bool tainted_p() const {
      if(extended_p()) return extended_header()->tainted_p();

      return RBX_MEMORY_TAINTED_P;
    }

    void set_tainted() {
      if(extended_p()) {
        extended_header()->set_tainted();
      } else {
        RBX_MEMORY_HEADER_TAINTED_SET;
      }
    }

    void unset_tainted() {
      if(extended_p()) {
        extended_header()->unset_tainted();
      } else {
        RBX_MEMORY_HEADER_TAINTED_UNSET;
      }
    }

    unsigned int locked_count() const {
      if(extended_p()) return extended_header()->locked_count();

      return RBX_MEMORY_LOCKED_COUNT_GET;
    }

    size_t object_id() const {
      if(extended_p()) return extended_header()->object_id();

      return RBX_MEMORY_OBJECT_ID_GET;
    }

    unsigned int type_specific() const {
      if(extended_p()) return extended_header()->type_specific();

      return RBX_MEMORY_TYPE_SPECIFIC_GET;
    }

    void type_specific(unsigned int value) {
      if(extended_p()) {
        extended_header()->type_specific(value);
      } else {
        RBX_MEMORY_HEADER_TYPE_SPECIFIC_SET(value);
      }
    }

    // Operations on a managed object.

    uintptr_t get_object_id() {
      uintptr_t id = object_id_counter.fetch_add(1);

      while(true) {
        if(id < RBX_MEMORY_OBJECT_ID_MAX) {
          if(extended_p()) {
            if(extended_header()->set_object_id(id)) {
              return id;
            }

            if(object_id() > 0) return object_id();
          } else {
            MemoryFlags h = header;
            MemoryFlags nh = RBX_MEMORY_OBJECT_ID_SET(id);

            if(header.compare_exchange_strong(h, nh)) {
              return id;
            } else if(object_id() != 0) {
              return object_id();
            }
          }
        }

        if(extended_p()) {
          ExtendedHeader* eh = extended_header();

          MemoryFlags h = header;
          ExtendedHeader* n = ExtendedHeader::create_object_id(h, eh);
          MemoryFlags nh = RBX_MEMORY_HEADER_EXTENDED_SET(n);

          if(header.compare_exchange_strong(h, nh)) {
            // TODO: dead_list.push_back(eh);
            return n->object_id();
          } else {
            if(extended_header()->object_id() != 0) {
              return extended_header()->object_id();
            }

            n->delete_header();
          }
        } else {
          MemoryFlags h = header;
          ExtendedHeader* n = ExtendedHeader::create_object_id(h);
          MemoryFlags nh = RBX_MEMORY_HEADER_EXTENDED_SET(n);

          if(header.compare_exchange_strong(h, nh)) {
            return n->object_id();
          } else {
            if(extended_header()->object_id() != 0) {
              return extended_header()->object_id();
            }

            n->delete_header();
          }
        }
      }
    }

    MemoryHandle* get_handle() {
      while(true) {
        if(extended_p()) {
          ExtendedHeader* eh = extended_header();

          if(MemoryHandle* handle = eh->get_handle()) {
            return handle;
          }

          MemoryFlags h = header;
          ExtendedHeader* n = ExtendedHeader::create_handle(h, eh);
          MemoryFlags nh = RBX_MEMORY_HEADER_EXTENDED_SET(n);

          if(header.compare_exchange_strong(h, nh)) {
            // TODO: dead_list.push_back(eh);
            return n->get_handle();
          } else {
            if(MemoryHandle* handle = extended_header()->get_handle()) {
              return handle;
            }

            n->delete_header();
          }
        } else {
          MemoryFlags h = header;
          ExtendedHeader* n = ExtendedHeader::create_handle(h);
          MemoryFlags nh = RBX_MEMORY_HEADER_EXTENDED_SET(n);

          if(header.compare_exchange_strong(h, nh)) {
            return n->get_handle();
          } else {
            if(MemoryHandle* handle = extended_header()->get_handle()) {
              return handle;
            }

            n->delete_header();
          }
        }
      }
    }

    int add_reference() {
      return 0;
    }

    void lock() {
    }

    bool try_lock() {
      return false;
    }

    void unlock() {
    }
  };

  /**
   * An InflatedHeader is used on the infrequent occasions when an Object needs
   * to store more metadata than can fit in the ObjectHeader HeaderWord struct.
   */
  class InflatedHeader {
    // Treat the header as either storage for the ObjectFlags, or as a pointer
    // to the next free InflatedHeader in the InflatedHeaders free list.
    union {
      capi::Handle* handle_;
      uintptr_t next_index_;
    };

    utilities::thread::Mutex mutex_;
    utilities::thread::Condition condition_;
    uint32_t owner_id_;
    int rec_lock_count_;

    uint32_t object_id_;
    unsigned int mark_;

  public:

    InflatedHeader()
      : handle_(NULL)
      , mutex_(false)
      , owner_id_(0)
      , rec_lock_count_(0)
      , object_id_(0)
      , mark_(-1)
    {}

    uintptr_t next() const {
      return next_index_;
    }

    uint32_t object_id(STATE) const {
      return object_id_;
    }

    void set_object_id(uint32_t id) {
      object_id_ = id;
    }

    void set_next(uintptr_t next_index) {
      next_index_ = next_index;
    }

    void clear() {
      handle_ = 0;
      owner_id_ = 0;
      rec_lock_count_ = 0;
      object_id_ = 0;
      mark_ = 0;
    }

    bool in_use_p() const {
      return mark_ > 0;
    }

    capi::Handle* handle(STATE) const {
      return handle_;
    }

    void set_handle(STATE, capi::Handle* handle) {
      atomic::write(&handle_, handle);
    }

    void clear_handle(STATE) {
      handle_ = NULL;
    }

    bool marked_p(unsigned int which) const {
      return (mark_ & which) == which;
    }

    void mark(Memory* om, unsigned int which) {
      mark_ = which;
    }

    bool update(STATE, HeaderWord header);
    void initialize_mutex(int thread_id, int count);
    LockStatus lock_mutex(STATE, ObjectHeader* obj, size_t us, bool interrupt);
    LockStatus lock_mutex_timed(STATE, ObjectHeader* obj, const struct timespec* ts, bool interrupt);
    LockStatus try_lock_mutex(STATE, ObjectHeader* obj);
    bool locked_mutex_p(STATE, ObjectHeader* obj);
    LockStatus unlock_mutex(STATE, ObjectHeader* obj);
    void unlock_mutex_for_terminate(STATE, ObjectHeader* obj);

    void wakeup(STATE, ObjectHeader* obj);

    private:
  };

  class DataHeader : public MemoryHeader {
    // Provides access to bytes beyond the header without reserving memory.
    void* __body__[0];

  public:

    DataHeader() = delete;
    DataHeader(const DataHeader&) = delete;
    DataHeader& operator= (const DataHeader&) = delete;

    ~DataHeader() = delete;

    static void initialize(
        MemoryHeader* obj, int thread_id, MemoryRegion region, object_type type)
    {
      MemoryHeader::initialize(obj, thread_id, region, type, true);
    }

    static size_t align(size_t bytes) {
      return (bytes + (sizeof(MemoryHeader*) - 1)) & ~(sizeof(MemoryHeader*) - 1);
    }

    static size_t bytes_to_fields(size_t bytes) {
      return (bytes - sizeof(DataHeader)) / sizeof(Object*);
    }

    // Called when the memory size is not static.
    size_t compute_size_in_bytes(VM* vm) const;

    size_t size_in_bytes(VM* vm) const {
      size_t size = TypeInfo::instance_sizes[type_id()];
      if(size != 0) {
        return size;
      } else {
        return compute_size_in_bytes(vm);
      }
    }

    void initialize_fields(native_int bytes) {
      void** fields = __body__;
      native_int field_count = bytes_to_fields(bytes);

      for(native_int i = 0; i < field_count; i++) {
        fields[i] = cNil;
      }
    }

    size_t body_in_bytes(VM* state) const {
      return size_in_bytes(state) - sizeof(DataHeader);
    }

    void** pointer_to_body() {
      return __body__;
    }
  };

  class ObjectHeader {
  private:
    HeaderWord header;

#ifdef RBX_TEST
  public:
#else
  protected:
#endif

    attr_accessor(klass, Class);
    attr_accessor(ivars, Object);

  private:
    // Provides access to bytes beyond the header without reserving memory.
    void* __body__[0];

  public:
    ObjectHeader() = delete;
    ObjectHeader(const ObjectHeader&) = delete;
    ObjectHeader& operator= (const ObjectHeader&) = delete;

    ~ObjectHeader() = delete;

  public:

    static size_t align(size_t bytes) {
      return (bytes + (sizeof(Object*) - 1)) & ~(sizeof(Object*) - 1);
    }

    static size_t bytes_to_fields(size_t bytes) {
      return (bytes - sizeof(ObjectHeader)) / sizeof(Object*);
    }

  public: // accessors for header members

    bool inflated_header_p() const {
      return header.f.meaning == eAuxWordInflated;
    }

    static InflatedHeader* header_to_inflated_header(STATE, HeaderWord header);
    static InflatedHeader* header_to_inflated_header(Memory* om, HeaderWord header);

    InflatedHeader* inflated_header(STATE) const {
      return header_to_inflated_header(state, header);
    }

    bool set_inflated_header(STATE, uint32_t ih_header, HeaderWord orig);

    /*
     * We return a copy here for safe reading. This means that
     * even though the reader might see slightly outdated information,
     * it will not see total garbage when the race here with inflated
     * the header.
     *
     * All code using these flags either checks flags that don't change
     * such as obj_type or uses this before attempting a CAS to modify
     * it. If there is a race the CAS will fail and will be reattempted
     * so it will work correctly.
     *
     * Don't use inflated_header_p() here since that checks the current
     * header state, but after returning it might be that the header
     * has been inflated in the mean while.
     */
    ObjectFlags flags() const {
      return header.f;
    }

    gc_zone zone() const {
      return flags().zone;
    }

    // Only called in non contended scenario's
    void set_zone(gc_zone zone) {
      header.f.zone = zone;
    }

    unsigned int age() const {
      return flags().age;
    }

    unsigned int inc_age() {
      return ++header.f.age;
    }

    void set_cycle(unsigned int cycle) {
      header.f.age = cycle;
    }

    void set_age(unsigned int age);

    /*
     * Method is only used on first object initialization so it's
     * safe to not use an atomic swap here. Changing an object type
     * after it was constructed is also a big no no anyway.
     */
    void set_obj_type(object_type type) {
      header.f.obj_type = type;
    }

    capi::Handle* handle(STATE);

    void set_handle(STATE, capi::Handle* handle) {
      if(inflated_header_p()) {
        inflated_header(state)->set_handle(state, handle);
      } else {
        rubinius::bug("Setting handle directly on not inflated header");
      }
    }

    void clear_handle(STATE);

    void set_handle_index(STATE, uintptr_t handle_index);

  public:

    void initialize_copy(Memory* om, Object* other, unsigned int age);

    /* Copies the body of +other+ into +this+ */
    void copy_body(VM* state, Object* other);

    /* Used to make an exact state copy of +this+ into +other* */
    void initialize_full_state(VM* vm, Object* other, unsigned int age);

    /* Clear the body of the object, by setting each field to cNil */
    void initialize_fields(native_int bytes) {
      void** fields = __body__;
      native_int field_count = bytes_to_fields(bytes);

      for(native_int i = 0; i < field_count; i++) {
        fields[i] = cNil;
      }
    }

    /* Initialize the objects data with the most basic info. This is done
     * right after an object is created.
     *
     * Can only be used when the caller is sure that the object doesn't
     * have an inflated header, which is true of any brand new (ie fresh)
     * objects.
     */
    void init_header(gc_zone loc, object_type type) {
      header.flags64 = 0;
      header.f.obj_type = type;
      header.f.zone = loc;
    }

    void** pointer_to_body() {
      return __body__;
    }

    /* It's the slow case, should be called only if there's no cached
     * instance size. */
    size_t slow_size_in_bytes(VM* vm) const;

    /* The whole point of this is inlining */
    size_t size_in_bytes(VM* vm) const {
      size_t size = TypeInfo::instance_sizes[type_id()];
      if(size != 0) {
        return size;
      } else {
        return slow_size_in_bytes(vm);
      }
    }

    size_t body_in_bytes(VM* state) const {
      return size_in_bytes(state) - sizeof(ObjectHeader);
    }

    bool reference_p() const {
      return __REFERENCE_P__(this);
    }

    bool young_object_p() const {
      return zone() == YoungObjectZone;
    }

    bool mature_object_p() const {
      return zone() == MatureObjectZone;
    }

    bool large_object_p() const {
      return flags().InLarge == 1;
    }

    bool forwarded_p() const {
      return flags().Forwarded == 1;
    }

    Object* forward() const {
      return _ivars_;
    }

    Class* reference_class() const {
      return _klass_;
    }

    /**
     *  Mark this Object forwarded by the GC.
     *
     *  Sets the forwarded flag and stores the given Object* in
     *  the _klass_ field where it can be reached. This object is
     *  no longer valid and should be accessed through the new
     *  Object* (but code outside of the GC framework should not
     *  really run into this much if at all.)
     *
     *  A forwarded object should never exist while the GC is running.
     */

    void set_forward(ObjectHeader* fwd) {
      header.f.Forwarded = 1;

      // DO NOT USE klass() because we need to get around the
      // write barrier!
      _ivars_ = reinterpret_cast<Object*>(fwd);
    }

    bool marked_p(unsigned int which) const {
      return (flags().Marked & which) == which;
    }

    bool scanned_p(unsigned int which) const {
      return flags().Marked == which + 1;
    }

    void mark(Memory* om, unsigned int which);
    void scanned();

    unsigned int which_mark() const {
      return flags().Marked;
    }

    void clear_mark() {
      header.f.Marked = 0;
    }

    bool pinned_p() const {
      return flags().Pinned == 1;
    }

    bool pin();
    void unpin();

    // Only used on object initialization.
    void set_pinned() {
      header.f.Pinned = 1;
    }

    bool in_immix_p() const {
      return flags().InImmix == 1;
    }

    // Only called in non contended scenario's
    void set_in_immix() {
      header.f.InImmix = 1;
    }

    // Only called in non contended scenario's
    void set_in_large() {
      header.f.InLarge = 1;
    }

    bool remembered_p() const {
      return flags().Remember == 1;
    }

    void set_remember();
    void clear_remember();
    void clear_lock_contended();

    bool is_frozen_p() const {
      return flags().Frozen == 1;
    }

    void set_frozen(int val=1);

    bool is_tainted_p() const {
      if(reference_p()) {
        return flags().Tainted == 1;
      }
      return false;
    }

    void set_tainted(int val=1);

    uint32_t object_id(STATE) const {
      // Pull this out into a local so that we don't see any concurrent
      // changes to header.
      HeaderWord tmp = header;

      switch(tmp.f.meaning) {
      case eAuxWordObjID:
        return tmp.f.aux_word;
      case eAuxWordInflated:
        return header_to_inflated_header(state, tmp)->object_id(state);
      default:
        return 0;
      }
    }

    void set_object_id(STATE, uint32_t id);

    LockStatus lock(STATE, size_t us=0, bool interrupt=true);
    LockStatus try_lock(STATE);
    bool locked_p(STATE);
    LockStatus unlock(STATE);
    void unlock_for_terminate(STATE);
    void unlock_object_after_fork(STATE);

    // Abort if unable to lock
    void hard_lock(STATE, size_t us=0);

    // Abort if unable to unlock
    void hard_unlock(STATE);

    void wait(STATE);

    bool nil_p() const {
      return (reinterpret_cast<intptr_t>(this) & NIL_MASK) == reinterpret_cast<intptr_t>(cNil);
    }

    bool true_p() const {
      return this == reinterpret_cast<ObjectHeader*>(cTrue);
    }

    bool false_p() const {
      return this == reinterpret_cast<ObjectHeader*>(cFalse);
    }

    bool undef_p() const {
      return this == reinterpret_cast<ObjectHeader*>(cUndef);
    }

    object_type type_id() const {
      return flags().obj_type;
    }

    bool check_type(object_type type) const {
      return reference_p() && flags().obj_type == type;
    }

    void validate() const {
      assert(!reference_p() || (type_id() > InvalidType && type_id() < LastObjectType));
    }

    friend class TypeInfo;
    friend class Memory;
  };
}

#endif
