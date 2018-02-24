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

#define CBOOL(v)    (((uintptr_t)(v) & FALSE_MASK) != (uintptr_t)cFalse)
#define RBOOL(v)    ((v) ? cTrue : cFalse)

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
   * little endian architecture. The implementation of the header operations
   * use simple integral values and are the same regardless of endianness (but
   * obviously, the actual memory layout of these values would depend on the
   * endianness of the particular platform).
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

  typedef uintptr_t MemoryFlags;

  static_assert(sizeof(MemoryFlags) == 8, "MemoryFlags must be 64 bits");

  enum MemoryRegion {
    eThreadRegion = 0,
    eFirstRegion,
    eSecondRegion,
    eLargeRegion,
  };

  struct HeaderField {
    int shift;
    int width;

    uintptr_t mask() const {
      return (1L << width) - 1;
    }

    uintptr_t max() const {
      return mask();
    }

    uintptr_t get(const MemoryFlags& header) const {
      return (header & (mask() << shift)) >> shift;
    }

    uintptr_t set(const MemoryFlags& header, uintptr_t value) const {
      return (header & ~(mask() << shift)) | ((value & mask()) << shift);
    }

    uintptr_t set(const MemoryFlags& header) const {
      return header | (mask() << shift);
    }

    uintptr_t unset(const MemoryFlags& header) const {
      return header & ~(mask() << shift);
    }
  };

  const HeaderField constexpr size_field          = { 0,  2 };
  const HeaderField constexpr extended_field      = { 0,  1 };
  const HeaderField constexpr forwarded_field     = { 1,  1 };
  const HeaderField constexpr thread_id_field     = { 2,  12 };
  const HeaderField constexpr region_field        = { 14, 2 };
  const HeaderField constexpr pinned_field        = { 16, 1 };
  const HeaderField constexpr visited_field       = { 17, 2 };
  const HeaderField constexpr marked_field        = { 19, 2 };
  const HeaderField constexpr scanned_field       = { 21, 1 };
  const HeaderField constexpr referenced_field    = { 22, 4 };
  const HeaderField constexpr type_id_field       = { 26, 10 };
  const HeaderField constexpr data_field          = { 36, 1 };
  const HeaderField constexpr frozen_field        = { 37, 1 };
  const HeaderField constexpr tainted_field       = { 38, 1 };
  const HeaderField constexpr locked_count_field  = { 39, 3 };
  const HeaderField constexpr object_id_field     = { 42, 20 };
  const HeaderField constexpr type_specific_field = { 62, 2 };

  /* Managed objects have a header. The header (a MemoryHeader) is a single, 64bit,
   * machine word that may be a MemoryFlags or pointer to an ExtendedHeader.
   *
   * The object's header field values may be single bit flags or multi-bit
   * values.  There are two classes of multi-bit values: 1. finite,
   * MemoryFlags-only; 2. finite, but expandable to a single machine word in
   * the 'words' vector in an ExtendedHeader.
   *
   * Header values have two operations: 1. get; 2. set. There are two
   * classes of set operations based on the two classes of multi-bit values:
   * 1. set bits in the MemoryFlags word; 2. add a value to the
   * ExtendedHeader 'words' vector.
   *
   * All changes to the managed object's header are compare-exchanged (ie
   * CAS) into the object's first word to ensure there are not pathological
   * read-update-write or write-write data race conditions.
   *
   * The values in the ExtendedHeader 'words' are stable pointers or values.
   * When the ExtendedHeader is changed to expand the 'words' or update the
   * MemoryFlags, the existing pointer values are unchanged, so any
   * references to these pointers are valid before and after the header
   * change. Only the MemoryFlags word in the MemoryHeader is updated.
   */

  struct MemoryHandle {
  };

  struct ExtendedHeaderWord {
    uintptr_t word;

    enum WordType {
      eUnsetWord = 1,
      eObjectID,
      eHandle,
      eRefCount,
      eLock,
    };

    ExtendedHeaderWord(unsigned int type = eUnsetWord)
      : word(type)
    {
    }

    unsigned int value_shift() const {
      return 0x3;
    }

    uintptr_t type_mask() const {
      return 0x7L;
    }

    uintptr_t ptr_mask() const {
      return ~type_mask();
    }

    bool unset_p() const {
      return (word & type_mask()) == eUnsetWord;
    }

    bool object_id_p() const {
      return (word & type_mask()) == eObjectID;
    }

    bool handle_p() const {
      return (word & type_mask()) == eHandle;
    }

    MemoryHandle* get_handle() const {
      if(handle_p()) {
        return reinterpret_cast<MemoryHandle*>(word & ptr_mask());
      }

      return nullptr;
    }

    bool referenced_p() const {
      return (word & type_mask()) == eRefCount;
    }

    uintptr_t get_referenced() const {
      if(referenced_p()) {
        return reinterpret_cast<uintptr_t>(word >> value_shift());
      }

      return 0;
    }

    void set_referenced(uintptr_t refcount) {
      word = (refcount << value_shift()) | eRefCount;
    }

    bool lock_p() const {
      return (word & type_mask()) == eLock;
    }
  };

  static_assert(sizeof(ExtendedHeaderWord) == sizeof(uintptr_t),
      "ExtendedHeaderWord structure must be the same size as uintptr_t");

  struct ExtendedHeader {
    MemoryFlags header;
    ExtendedHeaderWord words[0];

    ExtendedHeader(const MemoryFlags h, int size)
      : header(size_field.set(h, size))
    {
      ExtendedHeaderWord unset;

      words[0] = unset;
    }

    ExtendedHeader(const MemoryFlags h, const ExtendedHeader* eh, int size)
      : header(size_field.set(h, size))
    {
      for(int i = 0; i < eh->size(); i++) {
        words[i] = eh->words[i];
      }
    }

    ~ExtendedHeader() {
      for(int i = 0; i < size(); i++) {
        if(MemoryHandle* handle = words[i].get_handle()) {
          delete handle;
        }
      }
    }

    static ExtendedHeader* create(const MemoryFlags h) {
      uintptr_t* mem = new uintptr_t[2];

      return new(mem) ExtendedHeader(h, 1);
    }

    static ExtendedHeader* create(const MemoryFlags h, const ExtendedHeader* eh) {
      uintptr_t* mem = new uintptr_t[eh->size() + 2];

      return new(mem) ExtendedHeader(h, eh, eh->size() + 1);
    }

    static ExtendedHeader* create_copy(const MemoryFlags h, const ExtendedHeader* eh) {
      uintptr_t* mem = new uintptr_t[eh->size() + 1];

      return new(mem) ExtendedHeader(h, eh, eh->size());
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

    static ExtendedHeader* create_referenced(const MemoryFlags h, uintptr_t refcount) {
      ExtendedHeader* nh = create(h);
      nh->words[0].set_referenced(refcount);

      return nh;
    }

    static ExtendedHeader* create_referenced(
        const MemoryFlags h, const ExtendedHeader* eh, uintptr_t refcount)
    {
      ExtendedHeader* nh = create(h, eh);
      nh->words[eh->size()].set_referenced(refcount);

      return nh;
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
      return size_field.get(header);
    }

    MemoryHandle* get_handle() const {
      for(int i = 0; i < size(); i++) {
        if(MemoryHandle* handle = words[i].get_handle()) {
          return handle;
        }
      }

      return nullptr;
    }

    uintptr_t get_referenced() const {
      for(int i = 0; i < size(); i++) {
        if(uintptr_t refcount = words[i].get_referenced()) {
          return refcount;
        }
      }

      return 0;
    }

    void set_referenced(uintptr_t refcount) {
      for(int i = 0; i < size(); i++) {
        if(words[i].referenced_p()) {
          words[i].set_referenced(refcount);
        }
      }
    }
  };

  struct MemoryHeader {
    std::atomic<MemoryFlags> header;

    static std::atomic<uintptr_t> object_id_counter;

    static void bootstrap(STATE);

    void initialize(int thread_id, MemoryRegion region, object_type type, bool data) {
      MemoryFlags h = 0;

      header.store(thread_id_field.set(h, thread_id)
                   | region_field.set(h, region)
                   | type_id_field.set(h, type)
                   | data_field.set(h, data),
                   std::memory_order_release);
    }

    static void initialize(MemoryHeader* obj,
        int thread_id, MemoryRegion region, object_type type, bool data)
    {
      obj->initialize(thread_id, region, type, data);
    }

    static unsigned int max_thread_id() {
      return thread_id_field.max();
    }

    static unsigned int max_referenced() {
      return referenced_field.max();
    }

    static unsigned int max_type_id() {
      return type_id_field.max();
    }

    static unsigned int max_object_id() {
      return object_id_field.max();
    }

    static unsigned int max_locked_count() {
      return locked_count_field.max();
    }

    ExtendedHeader* extended_header() const {
      return const_cast<ExtendedHeader*>(
          reinterpret_cast<const ExtendedHeader*>(header.load() & ~0x7L));
    }

    MemoryFlags extended_flags(ExtendedHeader* h) const {
      MemoryFlags f = reinterpret_cast<MemoryFlags>(h);

      return reinterpret_cast<MemoryFlags>(extended_field.set(f));
    }

    bool extended_p() const {
      return extended_field.get(header);
    }

    uintptr_t get(const HeaderField field) const {
      if(extended_p()) {
        return field.get(extended_header()->header);
      } else {
        return field.get(header);
      }
    }

    void set(const HeaderField field, unsigned int value) {
      while(true) {
        MemoryFlags h = header;

        if(extended_p()) {
          ExtendedHeader* eh = ExtendedHeader::create_copy(
              field.set(h, value), extended_header());
          MemoryFlags nh = extended_flags(eh);

          if(header.compare_exchange_strong(h, nh)) return;

          eh->delete_header();
        } else {
          MemoryFlags nh = field.set(h, value);

          if(header.compare_exchange_strong(h, nh)) return;
        }
      }
    }

    void set(const HeaderField field) {
      while(true) {
        MemoryFlags h = header;

        if(extended_p()) {
          ExtendedHeader* eh = ExtendedHeader::create_copy(field.set(h), extended_header());
          MemoryFlags nh = extended_flags(eh);

          if(header.compare_exchange_strong(h, nh)) return;

          eh->delete_header();
        } else {
          MemoryFlags nh = field.set(h);

          if(header.compare_exchange_strong(h, nh)) return;
        }
      }
    }

    void unset(const HeaderField field) {
      while(true) {
        MemoryFlags h = header;

        if(extended_p()) {
          ExtendedHeader* eh = ExtendedHeader::create_copy(field.unset(h), extended_header());
          MemoryFlags nh = extended_flags(eh);

          if(header.compare_exchange_strong(h, nh)) return;

          eh->delete_header();
        } else {
          MemoryFlags nh = field.unset(h);

          if(header.compare_exchange_strong(h, nh)) return;
        }
      }
    }

    bool forwarded_p() const {
      return forwarded_field.get(header);
    }

    int thread_id() const {
      return get(thread_id_field);
    }

    void set_thread_id(uintptr_t id) {
      set(thread_id_field, id);
    }

    MemoryRegion region() const {
      return static_cast<MemoryRegion>(get(region_field));
    }

    void region(MemoryRegion region) {
      set(region_field, region);
    }

    bool pinned_p() const {
      return get(pinned_field);
    }

    void set_pinned() {
      set(pinned_field);
    }

    void unset_pinned() {
      unset(pinned_field);
    }

    bool visited_p(unsigned int flag) const {
      return get(visited_field) == flag;
    }

    void set_visited(unsigned int flag) {
      set(visited_field, flag);
    }

    bool marked_p(unsigned int mark) const {
      return get(marked_field) == mark;
    }

    void set_marked(unsigned int mark) {
      set(marked_field, mark);
    }

    bool scanned_p() const {
      return get(scanned_field);
    }

    void set_scanned() {
      set(scanned_field);
    }

    void unset_scanned() {
      unset(scanned_field);
    }

    unsigned int referenced() const {
      if(extended_p()) {
        ExtendedHeader* eh = extended_header();
        uintptr_t refcount = referenced_field.get(extended_header()->header);

        if(refcount < max_referenced()) {
          return refcount;
        } else {
          uintptr_t ext_refcount = eh->get_referenced();

          if(ext_refcount > 0) {
            return ext_refcount;
          } else {
            return refcount;
          }
        }
      } else {
        return referenced_field.get(header);
      }
    }

    object_type type_id() const {
      return static_cast<object_type>(get(type_id_field));
    }

    bool data_p() const {
      return get(data_field);
    }

    bool object_p() const {
      return !data_p();
    }

    bool reference_p() const {
      return __REFERENCE_P__(this);
    }

    bool frozen_p() const {
      return get(frozen_field);
    }

    void set_frozen() {
      set(frozen_field);
    }

    void unset_frozen() {
      unset(frozen_field);
    }

    bool tainted_p() const {
      return get(tainted_field);
    }

    void set_tainted() {
      set(tainted_field);
    }

    void unset_tainted() {
      unset(tainted_field);
    }

    uintptr_t object_id() const {
      return get(object_id_field);
    }

    unsigned int type_specific() const {
      return get(type_specific_field);
    }

    void type_specific(unsigned int value) {
      set(type_specific_field, value);
    }

    // Operations on a managed object.

    void lock(STATE);
    bool try_lock(STATE);
    void unlock(STATE);

    uintptr_t get_object_id() {
      uintptr_t id = object_id_counter.fetch_add(1);

      while(true) {
        MemoryFlags h = header;

        if(extended_p()) {
          ExtendedHeader* eh;
          ExtendedHeader* hh = extended_header();

          if(object_id_field.get(hh->header) > 0) {
            return object_id_field.get(hh->header);
          } else if(id < max_object_id()) {
            eh = ExtendedHeader::create_copy(object_id_field.set(hh->header, id), hh);
          } else {
            eh = ExtendedHeader::create_object_id(object_id_field.set(hh->header, id), hh);
          }

          MemoryFlags nh = extended_flags(eh);

          if(header.compare_exchange_strong(h, nh)) return id;

          eh->delete_header();
        } else {
          if(object_id_field.get(h) > 0) {
            return object_id_field.get(h);
          } else {
            MemoryFlags nh = object_id_field.set(h, id);

            if(header.compare_exchange_strong(h, nh)) return id;
          }
        }
      }
    }

    MemoryHandle* get_handle() {
      while(true) {
        MemoryFlags h = header;
        ExtendedHeader* eh;

        if(extended_p()) {
          if(MemoryHandle* handle = extended_header()->get_handle()) {
            return handle;
          }

          eh = ExtendedHeader::create_handle(h, extended_header());
        } else {
          eh = ExtendedHeader::create_handle(h);
        }

        MemoryFlags nh = extended_flags(eh);

        if(header.compare_exchange_strong(h, nh)) return eh->get_handle();

        eh->delete_header();
      }
    }

    int add_reference() {
      while(true) {
        MemoryFlags h = header;

        if(extended_p()) {
          ExtendedHeader* eh;
          ExtendedHeader* hh = extended_header();
          uintptr_t refcount = referenced_field.get(hh->header);

          if(refcount < max_referenced()) {
            eh = ExtendedHeader::create_copy(
                referenced_field.set(hh->header, refcount + 1), hh);
          } else {
            if(uintptr_t rc = hh->get_referenced()) {
              refcount = rc;

              eh = ExtendedHeader::create_copy(hh->header, hh);
              eh->set_referenced(refcount + 1);
            } else {
              eh = ExtendedHeader::create_referenced(hh->header, hh, refcount + 1);
            }
          }

          MemoryFlags nh = extended_flags(eh);

          if(header.compare_exchange_strong(h, nh)) return refcount + 1;

          eh->delete_header();
        } else {
          unsigned int refcount = referenced_field.get(h);

          if(refcount < max_referenced()) {
            MemoryFlags nh = referenced_field.set(h, refcount + 1);

            if(header.compare_exchange_strong(h, nh)) return refcount + 1;
          } else {
            ExtendedHeader* eh = ExtendedHeader::create_referenced(h, refcount + 1);

            MemoryFlags nh = extended_flags(eh);

            if(header.compare_exchange_strong(h, nh)) return refcount + 1;

            eh->delete_header();
          }
        }
      }
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
