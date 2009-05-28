#ifndef RBX_VM_OOP_HPP
#define RBX_VM_OOP_HPP

#include <cstddef>
#include <ctype.h>
#include <stdint.h>

#include "object_types.hpp"
#include "type_info.hpp"

namespace rubinius {

/* We use a variable length OOP tag system:
 * The tag represents 1 to 3 bits which uniquely
 * identify a data type.
 *
 *   1 == rest is a fixnum
 *  00 == rest is an object reference
 * 010 == rest is a boolean literal
 * 110 == rest is a symbol
*/

#define TAG_REF          0x0
#define TAG_REF_MASK     3

#define TAG_FIXNUM       0x1
#define TAG_FIXNUM_SHIFT 1
#define TAG_FIXNUM_MASK  1

#define TAG_BOOL         0x2

#define TAG_SYMBOL       0x6
#define TAG_SYMBOL_SHIFT 3
#define TAG_SYMBOL_MASK  7

#define APPLY_FIXNUM_TAG(v) ((Object*)(((intptr_t)(v) << TAG_FIXNUM_SHIFT) | TAG_FIXNUM))
#define STRIP_FIXNUM_TAG(v) (((intptr_t)v) >> TAG_FIXNUM_SHIFT)

#define APPLY_SYMBOL_TAG(v) ((Object*)(((intptr_t)(v) << TAG_SYMBOL_SHIFT) | TAG_SYMBOL))
#define STRIP_SYMBOL_TAG(v) (((intptr_t)v) >> TAG_SYMBOL_SHIFT)

#define REFERENCE_P(v) (((intptr_t)(v) & TAG_REF_MASK) == TAG_REF)
#define FIXNUM_P(v)    (((intptr_t)(v) & TAG_FIXNUM_MASK) == TAG_FIXNUM)
#define SYMBOL_P(v)    (((intptr_t)(v) & TAG_SYMBOL_MASK) == TAG_SYMBOL)

/* How many bits of data are available in fixnum, not including the sign. */
#define FIXNUM_WIDTH ((8 * sizeof(native_int)) - TAG_FIXNUM_SHIFT - 1)
#define FIXNUM_MAX   (((native_int)1 << FIXNUM_WIDTH) - 1)

/* This would naturally be (-(FIXNUM_MAX) - 1) considering the range of bits
 * and how twos-complement works. However, the libtommath library used by
 * Bignum does not store negative numbers in twos-complement. Consequently,
 * this value of FIXNUM_MIN allows for checking that a value is in the Fixnum
 * range merely by checking a count of the bits used to represent the number.
 */
#define FIXNUM_MIN   (-(FIXNUM_MAX))

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

/* NOTE if these change, be sure to update vm/capi/ruby.h, it contains
 * a private copy of these constants */

/* NOTE ALSO! the special clases array uses this bit pattern, so
 * if you change this, be sure to update the special class array! */
const int cFalse = 0x0aL;
#define Qfalse ((Object*)0x0aL)
const int cNil   = 0x1aL;
#define Qnil   ((Object*)0x1aL)
const int cTrue  = 0x12L;
#define Qtrue  ((Object*)0x12L)
const int cUndef = 0x22L;
#define Qundef ((Object*)0x22L)


// Indicates the mask to use to check if a value is ruby false.
// This mask matches both false and nil ONLY.
#define FALSE_MASK 0xf

#define FALSE_P(v) (((Object*)(v)) == Qfalse)
#define TRUE_P(v)  (((Object*)(v)) == Qtrue)
#define NIL_P(v)   (((Object*)(v)) == Qnil)
#define UNDEF_P(v) ((Object*)(v) == Qundef)
#define RTEST(v)   (((uintptr_t)(v) & FALSE_MASK) != (uintptr_t)Qfalse)

#define SIZE_OF_OBJECT ((size_t)(sizeof(ObjectHeader*)))

#define NUM_FIELDS(obj)                 ((obj)->num_fields())
#define SIZE_IN_BYTES_FIELDS(fel)       ((size_t)(sizeof(ObjectHeader) + \
      ((fel)*SIZE_OF_OBJECT)))
#define SIZE_IN_WORDS_FIELDS(fel)       (sizeof(ObjectHeader)/SIZE_OF_OBJECT + (fel))
#define SIZE_IN_BYTES(obj)              SIZE_IN_BYTES_FIELDS(obj->num_fields())
#define SIZE_OF_BODY(obj)               (obj->num_fields() * SIZE_OF_OBJECT)


  /* rubinius_object gc zone, takes up two bits */
  typedef enum
  {
    UnspecifiedZone  = 0,
    LargeObjectZone  = 1,
    MatureObjectZone = 2,
    YoungObjectZone  = 3,
  } gc_zone;

  /* the sizeof(class ObjectHeader) must an increment of the platform
     pointer size, so that the bytes located directly after a
     struct rubinius_object can hold a pointer which can be
     dereferenced. (an 32 bit platforms, pointers must be aligned
     on 32bit (word) boundaries. on 64 bit platforms, pointers probably
     have to be aligned on 64bit (double word) boundaries) */

  /* On a 32 bit platform, I expect ObjectHeader to take up
     4 + 4 + 4 = 12 bytes.
     on 64 bit platform,
     8 + 8 + 8 = 24 bytes.
     */

  class Class;
  class Object;
  class VM;

  class ObjectHeader {
    union {
      struct {
        object_type     obj_type_   : 8;
        gc_zone         zone        : 2;
        unsigned int    age         : 4;

        unsigned int Forwarded              : 1;
        unsigned int Remember               : 1;
        unsigned int Marked                 : 2;
        unsigned int RequiresCleanup        : 1;

        unsigned int RefsAreWeak            : 1;

        unsigned int InImmix                : 1;
        unsigned int Pinned                 : 1;
      };
      uintptr_t all_flags; // forces the width to pointer aligned
    };

#ifdef RBX_TEST
  public:
#else
  protected:
#endif

    Class* klass_;
    Object* ivars_;

  private:
    // Defined so ObjectHeader can easily access the data just beyond
    // it.
    void* __body__[];

  public:

    static size_t align(size_t bytes) {
      return (bytes + (sizeof(Object*) - 1)) & ~(sizeof(Object*) - 1);
    }

    static size_t bytes_to_fields(size_t bytes) {
      return (bytes - sizeof(ObjectHeader)) / sizeof(Object*);
    }

    void initialize_copy(Object* other, unsigned int age);

    /* Copies the body of +other+ into +this+ */
    void copy_body(VM* state, Object* other);

    /* Copies the flags of +this+ into +other+ */
    void copy_flags(Object* other);

    /* Clear the body of the object, by setting each field to Qnil */
    void clear_fields(size_t bytes);

    /* Clear the body of the object, setting it to all 0s */
    void clear_body_to_null(size_t bytes);

    /* Initialize the objects data with the most basic info. This is done
     * right after an object is created. */
    void init_header(gc_zone loc, object_type type) {
      all_flags = 0;
      obj_type_ = type;
      zone = loc;
    }

    /* It's the slow case, should be called only if there's no cached
     * instance size. */
    size_t slow_size_in_bytes(VM * state) const;

    /* The whole point of this is inlining */
    size_t size_in_bytes(VM* state) const {
      register size_t size = TypeInfo::instance_sizes[type_id()];
      if(size != 0) {
        return size;
      } else {
        return slow_size_in_bytes(state);
      }
    }

    size_t body_in_bytes(VM* state) {
      return size_in_bytes(state) - sizeof(ObjectHeader);
    }

    size_t total_size(VM* state) const {
      return size_in_bytes(state);
    }

    bool reference_p() const {
      return REFERENCE_P(this);
    }

    bool young_object_p() const {
      return zone == YoungObjectZone;
    }

    bool mature_object_p() const {
      return zone == MatureObjectZone;
    }

    bool forwarded_p() const {
      return Forwarded == 1;
    }

    void clear_forwarded() {
      Forwarded = 0;
    }

    Object* forward() {
      return ivars_;
    }

    /**
     *  Mark this Object forwarded by the GC.
     *
     *  Sets the forwarded flag and stores the given Object* in
     *  the klass_ field where it can be reached. This object is
     *  no longer valid and should be accessed through the new
     *  Object* (but code outside of the GC framework should not
     *  really run into this much if at all.)
     *
     *  A forwarded object should never exist while the GC is running.
     */

    void set_forward(Object* fwd) {
      Forwarded = 1;
      // DO NOT USE klass() because we need to get around the
      // write barrier!
      ivars_ = fwd;
    }

    bool marked_p() const {
      return Marked != 0;
    }

    bool marked_p(unsigned int which) const {
      return Marked == which;
    }

    void mark(unsigned int which=1) {
      Marked = which;
    }

    int which_mark() {
      return Marked;
    }

    void clear_mark() {
      Marked = 0;
    }

    bool pinned_p() {
      return Pinned == 1;
    }

    bool pin() {
      // Can't pin young objects!
      if(young_object_p()) return false;

      Pinned = 1;
      return true;
    }

    void unpin() {
      Pinned = 0;
    }

    bool in_immix_p() {
      return InImmix == 1;
    }

    void set_in_immix() {
      InImmix = 1;
    }

    bool remembered_p() {
      return Remember == 1;
    }

    void set_remember() {
      Remember = 1;
    }

    void clear_remember() {
      Remember = 0;
    }

    void set_requires_cleanup(int val) {
      RequiresCleanup = val;
    }

    bool requires_cleanup_p() {
      return RequiresCleanup == 1;
    }

    void set_refs_are_weak() {
      RefsAreWeak = 1;
    }

    bool refs_are_weak_p() {
      return RefsAreWeak == 1;
    }

    bool nil_p() const {
      return this == reinterpret_cast<ObjectHeader*>(Qnil);
    }

    bool undef_p() const {
      return this == reinterpret_cast<ObjectHeader*>(Qundef);
    }

    bool true_p() const {
      return this == reinterpret_cast<ObjectHeader*>(Qtrue);
    }

    bool false_p() const {
      return this == reinterpret_cast<ObjectHeader*>(Qfalse);
    }

    object_type type_id() const {
      return obj_type_;
    }

    bool check_type(object_type type) const {
      return reference_p() && obj_type_ == type;
    }

    friend class TypeInfo;
  };
}


#endif
