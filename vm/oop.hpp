#ifndef RBX_VM_OOP_HPP
#define RBX_VM_OOP_HPP

#include <stddef.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#include "config.h"
#include "object_types.hpp"
#include "type_info.hpp"
#include "detection.hpp"
#include "util/thread.hpp"

namespace thread {
  class Mutex;
}

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
#define TAG_REF_WIDTH    2

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

/* NOTE if these change, be sure to update vm/capi/include/ruby.h, it contains
 * a private copy of these constants */

/* NOTE ALSO! the special class array uses this bit pattern, so
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

// Some configuration flags
//
// Store the object id in the header on 64 bit platforms
#ifdef IS_X8664
#define RBX_OBJECT_ID_IN_HEADER
#endif

  /* rubinius_object gc zone, takes up two bits */
  typedef enum
  {
    UnspecifiedZone  = 0,
    LargeObjectZone  = 1,
    MatureObjectZone = 2,
    YoungObjectZone  = 3,
  } gc_zone;

  /* the sizeof(class ObjectHeader) must be a multiple of the platform
     pointer size, so that the bytes located directly after a
     struct rubinius_object can hold a pointer which can be
     dereferenced. (on 32 bit platforms, pointers must be aligned
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

  enum LockStatus {
    eUnlocked,
    eLocked,
    eLockTimeout,
    eLockInterrupted,
    eLockError
  };

  enum AuxWordMeaning {
    eAuxWordEmpty = 0,
    eAuxWordObjID = 1,
    eAuxWordLock =  2,
    eAuxWordInflated = 3
  };

  const static int AuxMeaningWidth = 2;
  const static int AuxMeaningMask  = 3;
  const static int cAuxLockTIDShift = 8;
  const static int cAuxLockRecCountMask = 0xff;
  const static int cAuxLockRecCountMax  = 0xff - 1;

  const static bool cDebugThreading = false;

  struct ObjectFlags {
#ifdef RBX_LITTLE_ENDIAN
    // inflated MUST be first, because rest is used as a pointer
    unsigned int meaning         : 2;
    object_type  obj_type        : 8;
    gc_zone      zone            : 2;
    unsigned int age             : 4;

    unsigned int Forwarded       : 1;
    unsigned int Remember        : 1;
    unsigned int Marked          : 2;

    unsigned int InImmix         : 1;
    unsigned int Pinned          : 1;

    unsigned int Frozen          : 1;
    unsigned int Tainted         : 1;
    unsigned int Untrusted       : 1;
    unsigned int LockContended   : 1;
    unsigned int unused          : 6;
#else
    unsigned int unused          : 6;
    unsigned int LockContended   : 1;
    unsigned int Untrusted       : 1;
    unsigned int Tainted         : 1;
    unsigned int Frozen          : 1;

    unsigned int Pinned          : 1;
    unsigned int InImmix         : 1;

    unsigned int Marked          : 2;
    unsigned int Remember        : 1;
    unsigned int Forwarded       : 1;

    unsigned int age             : 4;
    gc_zone      zone            : 2;
    object_type  obj_type        : 8;
    // inflated MUST be first, because rest is used as a pointer
    unsigned int meaning         : 2;
#endif

    uint32_t aux_word;
  };

  union HeaderWord {
    struct ObjectFlags f;
    uint64_t flags64;
    void* all_flags;

    bool atomic_set(HeaderWord& old, HeaderWord& nw);
  };

  namespace capi {
    class Handle;
  }


  /**
   * An InflatedHeader is used on the infrequent occasions when an Object needs
   * to store more metadata than can fit in the ObjectHeader HeaderWord struct.
   */
  class InflatedHeader {
    // Treat the header as either storage for the ObjectFlags, or as a pointer
    // to the next free InflatedHeader in the InflatedHeaders free list.
    union {
      ObjectFlags flags_;
      InflatedHeader* next_;
    };

    ObjectHeader* object_;
    capi::Handle* handle_;
    uint32_t object_id_;

    thread::Mutex mutex_;
    thread::Condition condition_;
    uint32_t owner_id_;
    int rec_lock_count_;

  public:

    InflatedHeader()
      : mutex_(false)
    {}

    ObjectFlags& flags() {
      return flags_;
    }

    InflatedHeader* next() {
      return next_;
    }

    ObjectHeader* object() {
      return object_;
    }

    uint32_t object_id() {
      return object_id_;
    }

    void set_object_id(uint32_t id) {
      object_id_ = id;
    }

    void set_next(InflatedHeader* next) {
      next_ = next;
    }

    void clear() {
      next_ = 0;
      object_ = 0;
      handle_ = 0;
      object_id_ = 0;
      rec_lock_count_ = 0;
      owner_id_ = 0;
    }

    bool used_p() {
      return object_ != 0;
    }

    void set_object(ObjectHeader* obj);
    void reset_object(ObjectHeader* obj) {
      object_ = obj;
    }

    bool marked_p(unsigned int which) const {
      return flags_.Marked == which;
    }

    capi::Handle* handle() {
      return handle_;
    }

    void set_handle(capi::Handle* handle) {
      handle_ = handle;
    }

    bool update(HeaderWord header);
    void initialize_mutex(int thread_id, int count);
    LockStatus lock_mutex(STATE, GCToken gct, size_t us=0);
    LockStatus lock_mutex_timed(STATE, GCToken gct, const struct timespec* ts);
    LockStatus try_lock_mutex(STATE, GCToken gct);
    bool locked_mutex_p(STATE, GCToken gct);
    LockStatus unlock_mutex(STATE, GCToken gct);
    void unlock_mutex_for_terminate(STATE, GCToken gct);

    void wakeup();
  };

  class ObjectHeader {
  private:
    HeaderWord header;

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
    void* __body__[0];

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

    static InflatedHeader* header_to_inflated_header(HeaderWord header) {
      uintptr_t untagged =
        reinterpret_cast<uintptr_t>(header.all_flags) & ~AuxMeaningMask;
      return reinterpret_cast<InflatedHeader*>(untagged);
    }

    InflatedHeader* inflated_header() const {
      return header_to_inflated_header(header);
    }

    bool set_inflated_header(InflatedHeader* ih);

    InflatedHeader* deflate_header();

    ObjectFlags& flags() const {
      if(inflated_header_p()) return inflated_header()->flags();
      return const_cast<ObjectFlags&>(header.f);
    }

    ObjectFlags& flags() {
      if(inflated_header_p()) return inflated_header()->flags();
      return header.f;
    }

    gc_zone zone() const {
      return flags().zone;
    }

    void set_zone(gc_zone zone) {
      flags().zone = zone;
    }

    unsigned int age() const {
      return flags().age;
    }

    unsigned int inc_age() {
      return flags().age++;
    }

    void set_age(unsigned int age) {
      flags().age = age;
    }

    void set_obj_type(object_type type) {
      flags().obj_type = type;
    }

  public:

    void initialize_copy(ObjectMemory* om, Object* other, unsigned int age);

    /* Copies the body of +other+ into +this+ */
    void copy_body(VM* state, Object* other);

    /* Used to make an exact state copy of +this+ into +other* */
    void initialize_full_state(VM* vm, Object* other, unsigned int age);

    /* Clear the body of the object, by setting each field to Qnil */
    void clear_fields(size_t bytes);

    /* Clear the body of the object, setting it to all 0s */
    void clear_body_to_null(size_t bytes);

    /* Initialize the objects data with the most basic info. This is done
     * right after an object is created. */
    void init_header(gc_zone loc, object_type type) {
      header.flags64 = 0;
      flags().obj_type = type;
      set_zone(loc);
    }

    void init_header(Class* cls, gc_zone loc, object_type type) {
      header.flags64 = 0;
      flags().obj_type = type;
      set_zone(loc);

      klass_ = cls;
      ivars_ = Qnil;
    }

    // Can only be used when the caller is sure that the object doesn't
    // have an inflated header, which is true of any brand new (ie fresh)
    // objects.
    void init_fresh_header(Class* cls, gc_zone loc, object_type type) {
      header.flags64 = 0;
      header.f.obj_type = type;
      header.f.zone = loc;

      klass_ = cls;
      ivars_ = Qnil;
    }

    void** pointer_to_body() {
      return __body__;
    }

    /* It's the slow case, should be called only if there's no cached
     * instance size. */
    size_t slow_size_in_bytes(VM* vm) const;

    /* The whole point of this is inlining */
    size_t size_in_bytes(VM* vm) const {
      register size_t size = TypeInfo::instance_sizes[type_id()];
      if(size != 0) {
        return size;
      } else {
        return slow_size_in_bytes(vm);
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
      return zone() == YoungObjectZone;
    }

    bool mature_object_p() const {
      return zone() == MatureObjectZone;
    }

    bool forwarded_p() const {
      return flags().Forwarded == 1;
    }

    void clear_forwarded() {
      flags().Forwarded = 0;
    }

    Object* forward() {
      return ivars_;
    }

    Object* ivars() {
      return ivars_;
    }

    Class* reference_class() {
      return klass_;
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

    void set_forward(ObjectHeader* fwd) {
      assert(flags().meaning == fwd->flags().meaning);

      // Wipe out the meaning since the only thing that matters
      // now is the forwarded bit
      header.f.meaning = eAuxWordEmpty;
      header.f.Forwarded = 1;

      // DO NOT USE klass() because we need to get around the
      // write barrier!
      ivars_ = reinterpret_cast<Object*>(fwd);
    }

    bool marked_p(unsigned int which) const {
      return flags().Marked == which;
    }

    void mark(unsigned int which) {
      flags().Marked = which;
    }

    int which_mark() {
      return flags().Marked;
    }

    void clear_mark() {
      flags().Marked = 0;
    }

    bool pinned_p() {
      return flags().Pinned == 1;
    }

    bool pin() {
      // Can't pin young objects!
      if(young_object_p()) return false;

      flags().Pinned = 1;
      return true;
    }

    void unpin() {
      flags().Pinned = 0;
    }

    bool in_immix_p() {
      return flags().InImmix == 1;
    }

    void set_in_immix() {
      flags().InImmix = 1;
    }

    bool remembered_p() {
      return flags().Remember == 1;
    }

    void set_remember() {
      flags().Remember = 1;
    }

    void clear_remember() {
      flags().Remember = 0;
    }

    bool is_frozen_p() {
      return flags().Frozen == 1;
    }

    void set_frozen(int val=1) {
      flags().Frozen = val;
    }

    bool is_tainted_p() {
      return flags().Tainted == 1;
    }

    void set_tainted(int val=1) {
      flags().Tainted = val;
    }

    bool is_untrusted_p() {
      return flags().Untrusted == 1;
    }

    void set_untrusted(int val=1) {
      flags().Untrusted = val;
    }

    uint32_t object_id() {
      // Pull this out into a local so that we don't see any concurrent
      // changes to header.
      HeaderWord tmp = header;

      switch(tmp.f.meaning) {
      case eAuxWordInflated:
        return header_to_inflated_header(tmp)->object_id();
      case eAuxWordObjID:
        return tmp.f.aux_word;
      default:
        return 0;
      }
    }

    void set_object_id(STATE, ObjectMemory* om, uint32_t id);

    LockStatus lock(STATE, GCToken gct, size_t us=0);
    LockStatus try_lock(STATE, GCToken gct);
    bool locked_p(STATE, GCToken gct);
    LockStatus unlock(STATE, GCToken gct);
    void unlock_for_terminate(STATE, GCToken gct);

    // Abort if unable to lock
    void hard_lock(STATE, GCToken gct, size_t us=0);

    // Abort if unable to unlock
    void hard_unlock(STATE, GCToken gct);

    void wait(STATE);

    bool nil_p() const {
      return this == reinterpret_cast<ObjectHeader*>(Qnil);
    }

    bool true_p() const {
      return this == reinterpret_cast<ObjectHeader*>(Qtrue);
    }

    bool false_p() const {
      return this == reinterpret_cast<ObjectHeader*>(Qfalse);
    }

    object_type type_id() const {
      return flags().obj_type;
    }

    bool check_type(object_type type) const {
      return reference_p() && flags().obj_type == type;
    }

    friend class TypeInfo;
    friend class ObjectMemory;
  };
}


#endif
