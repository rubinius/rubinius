#ifndef RBX_VM_OOP_HPP
#define RBX_VM_OOP_HPP

#include <cstddef>
#include <ctype.h>
#include <stdint.h>

#include "object_types.hpp"

namespace rubinius {

  /* OOP layout:
   * [30 (or 62) bits of data | 2 bits of tag]
   * if tag == 00, the whole thing is a pointer to a memory location.
   * if tag == 01, the data is a fixnum
   * if tag == 10, the data is a literal
   * if tag == 11, the data is any data, using the DATA_* macros
   */

#define TAG_MASK    0x3
#define TAG_SHIFT   2

#define TAG_REF     0x0
#define TAG_FIXNUM  0x1
#define TAG_LITERAL 0x2
#define TAG_DATA    0x3

#define TAG(v) (((intptr_t)v) & TAG_MASK)
#define APPLY_TAG(v, tag) ((OBJECT)(((intptr_t)v << TAG_SHIFT) | tag))
#define STRIP_TAG(v) (((intptr_t)v) >> TAG_SHIFT)

#define DATA_P(v) (TAG(v) == TAG_DATA)
#define FIXNUM_P(v) (TAG(v) == TAG_FIXNUM)

#define DATA_MASK   0x7
#define DATA_SHIFT  3

#define DATA_TAG_SYMBOL 0x3
#define DATA_TAG_CUSTOM 0x7

#define DATA_TAG(v) ((intptr_t)(v) & DATA_MASK)
#define DATA_APPLY_TAG(v, tag) (OBJECT)((v << DATA_SHIFT) | tag)
#define DATA_STRIP_TAG(v) (((intptr_t)v) >> DATA_SHIFT)

#define SYMBOL_P(v) (DATA_TAG(v) == DATA_TAG_SYMBOL)
#define CUSTOM_P(v) (DATA_TAG(v) == DATA_TAG_CUSTOM)

  /* How many bits of data are available in fixnum, not including
     the sign. */
#define FIXNUM_WIDTH ((8 * sizeof(native_int)) - TAG_SHIFT - 2)
#define FIXNUM_MAX (((native_int)1 << FIXNUM_WIDTH) - 1)
#define FIXNUM_MIN (-(FIXNUM_MAX) - 1)

  /* Standard Rubinius Representation

     Bit layout of special literals:

6:false    110   % 6 = 6
14:nil     1110   % 6 = 6
10:true    1010   % 6 = 2
18:undef  10010   % 6 = 2


false and nil share the same base bit pattern, allowing RTEST
to be a simple test for that bit pattern.
*/

#define FALSE_P(v) ((OBJECT)(v) == (OBJECT)Qfalse)
#define TRUE_P(v) ((OBJECT)(v) == (OBJECT)Qtrue)
#define NIL_P(v) ((OBJECT)(v) == (OBJECT)Qnil)
#define UNDEF_P(v) ((OBJECT)(v) == (OBJECT)Qundef)
#define RTEST(v) (((uintptr_t)(v) & 0x7) != 0x6)

#define REFERENCE_P(v) (TAG(v) == TAG_REF)

#define INDEXED(obj) (REFERENCE_P(obj) && !obj->StoresBytes)
#define STORE_BYTES(obj) (REFERENCE_P(obj) && obj->StoresBytes)

#define SIZE_OF_OBJECT ((size_t)(sizeof(ObjectHeader*)))

#define NUM_FIELDS(obj)                 (obj->field_count)
#define SET_NUM_FIELDS(obj, fel)        (obj->field_count = fel)
#define SIZE_IN_BYTES_FIELDS(fel)       ((size_t)(sizeof(ObjectHeader) + \
      (fel*SIZE_OF_OBJECT)))
#define SIZE_IN_WORDS_FIELDS(fel)       (sizeof(ObjectHeader)/SIZE_OF_OBJECT + fel)
#define SIZE_IN_BYTES(obj)              SIZE_IN_BYTES_FIELDS(obj->num_fields())
#define SIZE_OF_BODY(obj)               (obj->num_fields() * SIZE_OF_OBJECT)

#define Qfalse ((Object*)6L)
#define Qnil   ((Object*)14L)
#define Qtrue  ((Object*)10L)
#define Qundef ((Object*)18L)

  /* rubinius_object gc zone, takes up two bits */
  typedef enum
  {
    UnspecifiedZone  = 0,
    MatureObjectZone = 1,
    YoungObjectZone  = 2,
    LargeObjectZone  = 3,
  } gc_zone;

  typedef size_t hashval;

  /* the sizeof(class ObjectHeader) must an increment of the platform 
     pointer size, so that the bytes located directly after a
     struct rubinius_object can hold a pointer which can be
     dereferenced. (an 32 bit platforms, pointers must be aligned
     on 32bit (word) boundaries. on 64 bit platforms, pointers probably
     have to be aligned on 64bit (double word) boundaries) */

  /* On a 32 bit platform, I expect rubinius_object to take up
     4 + 4 + 4 + 4 = 16 bytes.
     on 64 bit platform,
     4 + 4 + 8 + 8 = 24 bytes.
     */

  class Class;
  class Object;

  class ObjectHeader {
  public:
    union {
      struct {
        object_type     obj_type    : 6;
        gc_zone         zone        : 2;
        unsigned int    age         : 3;

        unsigned int Forwarded              : 1;
        unsigned int Remember               : 1;
        unsigned int Marked                 : 1;
        unsigned int ForeverYoung           : 1;
        unsigned int StoresBytes            : 1;
        unsigned int RequiresCleanup        : 1;
        unsigned int IsBlockContext         : 1;
        unsigned int IsMeta                 : 1;

        unsigned int IsTainted              : 1;
        unsigned int IsFrozen               : 1;
        unsigned int RefsAreWeak            : 1;
      };
      uint32_t all_flags;
    };
    Class* klass_;
    Object* ivars_;

  private:
    // The number of fields this object uses.
    uint32_t field_count;

    // Defined so ObjectHeader can easily access the data just beyond
    // it.
    void* __body__[];

  public:
    /* Initialize the objects data with the most basic info. This is done
     * right after an object is created. */
    void init(gc_zone zone, size_t fields);

    void initialize_copy(Object* other, unsigned int age);

    /* Copies the body of +other+ into +this+ */
    void copy_body(Object* other);

    /* Clear the body of the object, by setting each field to Qnil */
    void clear_fields();

    /* Clear the body of the object, setting it to all 0s */
    void clear_body_to_null();

    uint32_t num_fields() {
      return field_count;
    }

    size_t size_in_bytes() {
      return SIZE_IN_BYTES(this);
    }

    size_t body_in_bytes() {
      return num_fields() * sizeof(ObjectHeader);
    }

    bool reference_p() {
      return REFERENCE_P(this);
    }

    bool stores_bytes_p() {
      return StoresBytes;
    }

    bool stores_references_p() {
      return !StoresBytes;
    }

    bool young_object_p() {
      return zone == YoungObjectZone;
    }

    bool mature_object_p() {
      return zone == MatureObjectZone;
    }

    bool forwarded_p() {
      return Forwarded == 1;
    }

    Object* forward() {
      return (Object*)klass_;
    }

    bool marked_p() {
      return Marked == 1;
    }

    void mark() {
      Marked = 1;
    }

    void clear_mark() {
      Marked = 0;
    }

    bool nil_p() {
      return this == reinterpret_cast<ObjectHeader*>(Qnil);
    }

    bool undef_p() {
      return this == reinterpret_cast<ObjectHeader*>(Qundef);
    }

    bool true_p() {
      return this == reinterpret_cast<ObjectHeader*>(Qtrue);
    }

    bool false_p() {
      return this == reinterpret_cast<ObjectHeader*>(Qfalse);
    }

    bool check_type(object_type type) {
      return reference_p() && obj_type == type;
    }

  };

  /* Object access, lowest level. These read and set fields of an OBJECT
   * directly. They're built on to integrate with the GC properly. */

#define CLEAR_FLAGS(obj)     (obj)->all_flags = 0
#define stack_context_p(obj) ((obj)->gc_zone == UnspecifiedZone)
#define SET_FORWARDED(obj)   (obj)->Forwarded = TRUE
#define FORWARDED_P(obj)     ((obj)->Forwarded)

#define AGE(obj)           (obj->copy_count)
#define CLEAR_AGE(obj)     (obj->copy_count = 0)
#define INCREMENT_AGE(obj) (obj->copy_count++)

}


#endif
