#ifndef RBX_VM_OOP_HPP
#define RBX_VM_OOP_HPP

#include <stddef.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>
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
namespace utilities {
namespace thread {
  class Mutex;
}
}

/* We use a variable length OOP tag system:
 * The tag represents 1 to 3 bits which uniquely
 * identify a data type.
 *
 *   1 == rest is a fixnum
 *  00 == rest is an object reference
 * 010 == rest is a boolean literal
 * 110 == rest is a symbol
 *
 * NOTE: If these definitions change, the definitions for the C-API must be
 * updated in the configure script.
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

/* Do not use these macros in code. They define the bit patterns for the
 * various object types and are used to define predicates. Use the predicates
 * (ie reference_p(), fixnum_p(), symbol_p()) directly.
 */
#define __REFERENCE_P__(v) (((intptr_t)(v) & TAG_REF_MASK) == TAG_REF)
#define __FIXNUM_P__(v)    (((intptr_t)(v) & TAG_FIXNUM_MASK) == TAG_FIXNUM)
#define __SYMBOL_P__(v)    (((intptr_t)(v) & TAG_SYMBOL_MASK) == TAG_SYMBOL)

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

/* NOTE ALSO! the special class array uses this bit pattern, so
 * if you change this, be sure to update the special class array! */
Object* const cFalse = reinterpret_cast<Object*>(0x0aL);
Object* const cNil   = reinterpret_cast<Object*>(0x1aL);
Object* const cTrue  = reinterpret_cast<Object*>(0x12L);
Object* const cUndef = reinterpret_cast<Object*>(0x22L);

// Indicates the mask to use to check if a value is ruby false.
// This mask matches both false and nil ONLY.
#define FALSE_MASK 0xf

#define CBOOL(v)                    (((uintptr_t)(v) & FALSE_MASK) != (uintptr_t)cFalse)
#define RBOOL(v)                    ((v) ? cTrue : cFalse)

#define SIZE_OF_OBJECT              ((size_t)(sizeof(ObjectHeader*)))

#define NUM_FIELDS(obj)             ((obj)->num_fields())
#define SIZE_IN_BYTES_FIELDS(fel)   ((size_t)(sizeof(ObjectHeader) + \
                                        ((fel)*SIZE_OF_OBJECT)))
#define SIZE_IN_WORDS_FIELDS(fel)   (sizeof(ObjectHeader)/SIZE_OF_OBJECT + (fel))
#define SIZE_IN_BYTES(obj)          SIZE_IN_BYTES_FIELDS(obj->num_fields())
#define SIZE_OF_BODY(obj)           (obj->num_fields() * SIZE_OF_OBJECT)

// Some configuration flags

  /* rubinius_object gc zone, takes up two bits */
  typedef enum
  {
    UnspecifiedZone  = 0,
    MatureObjectZone = 1,
    YoungObjectZone  = 2,
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
    eAuxWordEmpty  = 0,
    eAuxWordObjID  = 1,
    eAuxWordLock   = 2,
    eAuxWordHandle = 3
  };

  const static int InflatedMask  = 1;
  const static int cAuxLockTIDShift = 8;
  const static int cAuxLockRecCountMask = 0xff;
  const static int cAuxLockRecCountMax  = 0xff - 1;

  const static bool cDebugThreading = false;

  struct ObjectFlags {
#ifdef RBX_LITTLE_ENDIAN
    // inflated MUST be first, because rest is used as a pointer
    unsigned int inflated        : 1;
    object_type  obj_type        : 8;
    unsigned int meaning         : 2;
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
    unsigned int unused          : 5;
#else
    unsigned int unused          : 5;
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
    unsigned int meaning         : 2;
    object_type  obj_type        : 8;
    // inflated MUST be first, because rest is used as a pointer
    unsigned int inflated        : 1;
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
      uintptr_t next_index_;
    };

    ObjectHeader* object_;
    capi::Handle* handle_;
    uint32_t object_id_;

    utilities::thread::Mutex mutex_;
    utilities::thread::Condition condition_;
    uint32_t owner_id_;
    int rec_lock_count_;

  public:

    InflatedHeader()
      : mutex_(false)
    {}

    ObjectFlags flags() {
      return flags_;
    }

    uintptr_t next() const {
      return next_index_;
    }

    ObjectHeader* object() const {
      return object_;
    }

    uint32_t object_id() const {
      return object_id_;
    }

    void set_object_id(uint32_t id) {
      object_id_ = id;
    }

    void set_next(uintptr_t next_index) {
      next_index_ = next_index;
    }

    void clear() {
      next_index_ = 0;
      object_ = 0;
      handle_ = 0;
      object_id_ = 0;
      rec_lock_count_ = 0;
      owner_id_ = 0;
    }

    bool in_use_p() const {
      return object_ != 0;
    }

    void set_object(ObjectHeader* obj);
    void reset_object(ObjectHeader* obj) {
      object_ = obj;
    }

    bool marked_p(unsigned int which) const {
      return flags_.Marked == which;
    }

    capi::Handle* handle(STATE) const {
      return handle_;
    }

    void set_handle(STATE, capi::Handle* handle) {
      atomic::write(&handle_, handle);
    }

    unsigned int inc_age() {
      return flags_.age++;
    }

    void set_age(unsigned int age) {
      flags_.age = age;
    }

    void mark(unsigned int which) {
      flags_.Marked = which;
    }

    void clear_mark() {
      flags_.Marked = 0;
    }

    bool pin() {
      flags_.Pinned = 1;
      return true;
    }

    void unpin() {
      flags_.Pinned = 0;
    }

    void set_in_immix() {
      flags_.InImmix = 1;
    }

    void set_zone(gc_zone zone) {
      flags_.zone = zone;
    }

    void set_lock_contended() {
      flags_.LockContended = 1;
    }

    void clear_lock_contended() {
      flags_.LockContended = 0;
    }

    void set_remember() {
      flags_.Remember = 1;
    }

    void clear_remember() {
      flags_.Remember = 0;
    }

    void set_frozen(int val = 1) {
      flags_.Frozen = val;
    }

    void set_tainted(int val = 1) {
      flags_.Tainted = val;
    }

    void set_untrusted(int val = 1) {
      flags_.Untrusted = val;
    }

    bool update(STATE, HeaderWord header);
    void initialize_mutex(int thread_id, int count);
    LockStatus lock_mutex(STATE, GCToken gct, size_t us, bool interrupt);
    LockStatus lock_mutex_timed(STATE, GCToken gct, const struct timespec* ts, bool interrupt);
    LockStatus try_lock_mutex(STATE, GCToken gct);
    bool locked_mutex_p(STATE, GCToken gct);
    LockStatus unlock_mutex(STATE, GCToken gct);
    void unlock_mutex_for_terminate(STATE, GCToken gct);

    void wakeup();

    private:


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
      return header.f.inflated;
    }

    static InflatedHeader* header_to_inflated_header(HeaderWord header) {
      uintptr_t untagged =
        reinterpret_cast<uintptr_t>(header.all_flags) & ~InflatedMask;
      return reinterpret_cast<InflatedHeader*>(untagged);
    }

    InflatedHeader* inflated_header() const {
      return header_to_inflated_header(header);
    }

    bool set_inflated_header(STATE, InflatedHeader* ih);

    InflatedHeader* deflate_header();

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
      HeaderWord copy = header;
      if(copy.f.inflated) {
        return inflated_header()->flags();
      }
      return copy.f;
    }

    ObjectFlags flags() {
      HeaderWord copy = header;
      if(copy.f.inflated) {
        return inflated_header()->flags();
      }
      return copy.f;
    }

    gc_zone zone() const {
      return flags().zone;
    }

    void set_zone(gc_zone zone);

    unsigned int age() const {
      return flags().age;
    }

    unsigned int inc_age();
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
        inflated_header()->set_handle(state, handle);
      } else {
        rubinius::bug("Setting handle directly on not inflated header");
      }
    }

    void clear_handle(STATE);

    void set_handle_index(STATE, uintptr_t handle_index);

  public:

    void initialize_copy(ObjectMemory* om, Object* other, unsigned int age);

    /* Copies the body of +other+ into +this+ */
    void copy_body(VM* state, Object* other);

    /* Used to make an exact state copy of +this+ into +other* */
    void initialize_full_state(VM* vm, Object* other, unsigned int age);

    /* Clear the body of the object, by setting each field to cNil */
    void clear_fields(size_t bytes);

    /* Clear the body of the object, setting it to all 0s */
    void clear_body_to_null(size_t bytes);

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

    void init_header(Class* cls, gc_zone loc, object_type type) {
      header.flags64 = 0;
      header.f.obj_type = type;
      header.f.zone = loc;

      klass_ = cls;
      ivars_ = cNil;
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

    size_t body_in_bytes(VM* state) const {
      return size_in_bytes(state) - sizeof(ObjectHeader);
    }

    size_t total_size(VM* state) const {
      return size_in_bytes(state);
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

    bool forwarded_p() const {
      return flags().Forwarded == 1;
    }

    Object* forward() const {
      return ivars_;
    }

    Object* ivars() const {
      return ivars_;
    }

    Class* reference_class() const {
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

    void mark(unsigned int which);

    int which_mark() const {
      return flags().Marked;
    }

    void clear_mark();

    bool pinned_p() const {
      return flags().Pinned == 1;
    }

    bool pin();
    void unpin();

    bool in_immix_p() const {
      return flags().InImmix == 1;
    }

    void set_in_immix();

    bool remembered_p() const {
      return flags().Remember == 1;
    }

    void set_remember();
    void clear_remember();
    void set_lock_contended();
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

    bool is_untrusted_p() const {
      if(reference_p()) {
        return flags().Untrusted == 1;
      }
      return false;
    }

    void set_untrusted(int val=1);

    uint32_t object_id() {
      // Pull this out into a local so that we don't see any concurrent
      // changes to header.
      HeaderWord tmp = header;
      if(tmp.f.inflated) {
        return header_to_inflated_header(tmp)->object_id();
      }

      switch(tmp.f.meaning) {
      case eAuxWordObjID:
        return tmp.f.aux_word;
      default:
        return 0;
      }
    }

    void set_object_id(STATE, ObjectMemory* om, uint32_t id);

    LockStatus lock(STATE, GCToken gct, size_t us=0, bool interrupt=true);
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
      return this == reinterpret_cast<ObjectHeader*>(cNil);
    }

    bool true_p() const {
      return this == reinterpret_cast<ObjectHeader*>(cTrue);
    }

    bool false_p() const {
      return this == reinterpret_cast<ObjectHeader*>(cFalse);
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
