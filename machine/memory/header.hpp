#ifndef RBX_MEMORY_HEADER_HPP
#define RBX_MEMORY_HEADER_HPP

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <atomic>
#include <mutex>
#include <thread>

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "config.h"
#include "object_types.hpp"
#include "type_info.hpp"
#include "detection.hpp"
#include "bug.hpp"

#ifdef VALUE
#undef VALUE
#endif

#define VALUE intptr_t

struct RArray;
struct RString;
struct RData;
struct RTypedData;
struct RFloat;
struct RFile;
struct rb_io_t;
typedef struct rb_io_t RIO;

namespace rubinius {
  class Array;
  class Class;
  class Data;
  class Fixnum;
  class Float;
  class IO;
  class Object;
  class String;

  struct MemoryHeader;
  struct MemoryHandle;

/* We use a variable length pointer tag system: The tag represents 1 to 3 bits
 * which uniquely identify a data type.
 *
 *   1 == rest is a fixnum
 * 000 == rest is a managed object reference
 * 100 == rest is a handle pointer
 * 010 == rest is a boolean literal
 * 110 == rest is a symbol
 *
 * NOTE: If these definitions change, the definitions for the C-API must be
 * updated in the configure script.
*/

#define TAG_REF          0x0L
#define TAG_REF_MASK     7
#define TAG_REF_WIDTH    3

#define TAG_HANDLE       0x4L
#define TAG_HANDLE_MASK  7
#define TAG_HANDLE_WIDTH 3

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

#define APPLY_HANDLE_TAG(v) (((intptr_t)(v) & ~TAG_HANDLE_MASK) | TAG_HANDLE)
#define STRIP_HANDLE_TAG(v) ((MemoryHandle*)((intptr_t)(v) & ~TAG_HANDLE_MASK))

/* Do not use these macros in code. They define the bit patterns for the
 * various object types and are used to define predicates. Use the predicates
 * (ie reference_p(), fixnum_p(), symbol_p()) directly.
 */
#define __REFERENCE_P__(v)  ((v) && (((intptr_t)(v) & TAG_REF_MASK) == TAG_REF))
#define __HANDLE_P__(v)     (((intptr_t)(v) & TAG_HANDLE_MASK) == TAG_HANDLE)
#define __FIXNUM_P__(v)     (((intptr_t)(v) & TAG_FIXNUM_MASK) == TAG_FIXNUM)
#define __SYMBOL_P__(v)     (((intptr_t)(v) & TAG_SYMBOL_MASK) == TAG_SYMBOL)

/* How many bits of data are available in fixnum, not including the sign. */
#define FIXNUM_MAX_WIDTH  ((8 * sizeof(intptr_t)) - TAG_FIXNUM_SHIFT - 1)
#define FIXNUM_MIN_WIDTH  (FIXNUM_MAX_WIDTH + 1)

#define FIXNUM_MAX        (((intptr_t)1 << FIXNUM_MAX_WIDTH) - 1)
#define FIXNUM_MIN        (-1 - FIXNUM_MAX)

#define FIXNUM_NEG_ONE    (Object*)(~0UL)

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
#define NIL_BITS      0x1aL
#define NIL_MASK      0x1fL

#define NIL_IP_SHIFT  0x5L
#define NIL_IP_MASK   0xffffL

#define NIL_ID_SHIFT  0x15L
#define NIL_ID_MASK   0xffffffffL

#define APPLY_NIL_TAG(id,ip)  (0x0L \
                                | ((id & NIL_ID_MASK) << NIL_ID_SHIFT) \
                                | ((ip & NIL_IP_MASK) << NIL_IP_SHIFT) \
                                | NIL_BITS)
#define NIL_TAG_ID(n)         ((reinterpret_cast<intptr_t>(n) >> NIL_ID_SHIFT) & NIL_ID_MASK)
#define NIL_TAG_IP(n)         ((reinterpret_cast<intptr_t>(n) >> NIL_IP_SHIFT) & NIL_IP_MASK)

#define NIL_EQUAL_P(a,b)      ((a)->nil_p() && (b)->nil_p())

#define CBOOL(v)    (((uintptr_t)(v) & FALSE_MASK) != (uintptr_t)cFalse)
#define RBOOL(v)    ((v) ? cTrue : cFalse)


  typedef std::vector<Object*> ObjectArray;

  const static bool cDebugThreading = false;

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
   * The MemoryHeaderBits struct and MemoryHeader::header_bits() method are
   * provided for easier debugging but are unused in normal code.
   */

  typedef uintptr_t MemoryFlags;

  static_assert(sizeof(uintptr_t) == sizeof(uint64_t), "uintptr_t must be 64 bits");
  static_assert(sizeof(MemoryFlags) == sizeof(uintptr_t), "MemoryFlags must be sizeof uintptr_t");

  typedef struct MemoryHeaderBits {
    // Header status flags
    unsigned int extended;         // : 1;
    unsigned int forwarded;        // : 1;

    // Memory flags
    unsigned int thread_id;        // : 11;
    unsigned int region;           // : 2;
    unsigned int referenced;       // : 4;
    unsigned int weakref;          // : 1;
    unsigned int finalizer;        // : 1;
    unsigned int remembered;       // : 1;

    // Graph flags
    unsigned int visited;          // : 2;

    // Garbage collector flags
    unsigned int marked;           // : 2;
    unsigned int scanned;          // : 1;

    // Data type flags
    object_type type_id;           // : 9;
    unsigned int data;             // : 1;

    // Memory object flags
    unsigned int frozen;           // : 1;
    unsigned int tainted;          // : 1;
    unsigned int locked_count;     // : 3;
    unsigned int lock_extended;
    unsigned int lock_owner;
    unsigned int object_id;        // : 19;
    unsigned int type_specific;    // : 2;

    // Pinned is a type-specific bit and should only be used by ByteArray
    unsigned int pinned;           // : 1;

    MemoryHeaderBits(const MemoryHeader* header);
  } MemoryHeaderBits;

  enum MemoryRegion {
    eThreadRegion = 0,
    eFirstRegion,
    eSecondRegion,
    eThirdRegion,
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
  const HeaderField constexpr thread_id_field     = { 2,  11 };
  const HeaderField constexpr region_field        = { 13, 2 };
  const HeaderField constexpr referenced_field    = { 15, 4 };
  const HeaderField constexpr weakref_field       = { 19, 1 };
  const HeaderField constexpr finalizer_field     = { 20, 1 };
  const HeaderField constexpr remembered_field    = { 21, 1 };
  const HeaderField constexpr visited_field       = { 22, 2 };
  const HeaderField constexpr marked_field        = { 24, 2 };
  const HeaderField constexpr scanned_field       = { 26, 1 };
  const HeaderField constexpr type_id_field       = { 27, 9 };
  const HeaderField constexpr data_field          = { 36, 1 };
  const HeaderField constexpr frozen_field        = { 37, 1 };
  const HeaderField constexpr tainted_field       = { 38, 1 };
  const HeaderField constexpr locked_count_field  = { 39, 3 };
  const HeaderField constexpr object_id_field     = { 42, 20 };
  const HeaderField constexpr type_specific_field = { 62, 2 };

  // Pinned is a type-specific bit and should only be used by ByteArray
  const HeaderField constexpr pinned_field        = { 62, 1 };

  // TODO: reclaim from tainted or frozen field when those are removed
  const HeaderField constexpr zombie_field        = { 63, 1 };

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
    enum HandleType {
      eReleased,
      eObject,
      eRArray,
      eRString,
      eRData,
      eRFloat,
      eRIO,
      eRFile
    };

    attr_field(type, HandleType);
    attr_field(accesses, unsigned int);
    attr_field(cycles, unsigned int);
    attr_field(object, Object*);
    attr_field(data, void*);

    MemoryHandle(Object* object)
      : _type_(eObject)
      , _accesses_(1)
      , _cycles_(0)
      , _object_(object)
      , _data_(nullptr)
    {
    }

    virtual ~MemoryHandle();

    static MemoryHandle* from(VALUE value);
    static VALUE value(Object* object);
    static Object* object(VALUE value);

    template<typename T>
      static T* object(VALUE value);

    template<typename T>
      static T* try_as(VALUE value);

    VALUE as_value() const {
      return reinterpret_cast<VALUE>(APPLY_HANDLE_TAG(this));
    }

    bool valid_p() const {
      switch(_type_) {
        case eObject:
        case eRArray:
        case eRString:
        case eRData:
        case eRFloat:
        case eRIO:
        case eRFile:
          return true;
        default:
          return false;
      }
    }

    void set_data(void* data, HandleType type) {
      this->type(type);
      this->data(data);
    }

    void unset_accesses() {
      _accesses_ = 0;
    }

    void access() {
      _accesses_++;
    }

    void cycle() {
      _cycles_++;
    }

    bool object_type_p() const {
      return type() == eObject;
    }

    bool rarray_p() const {
      return type() == eRArray;
    }

    RArray* get_rarray(STATE);
    void read_rarray(STATE);
    void write_rarray(STATE);

    void set_rarray(RArray* rarray) {
      set_data(rarray, eRArray);
    }

    bool rstring_p() const {
      return type() == eRString;
    }

    RString* get_rstring(STATE);
    void read_rstring(STATE);
    void write_rstring(STATE);

    void set_rstring(RString* rstring) {
      set_data(rstring, eRString);
    }

    bool rdata_p() const {
      return type() == eRData;
    }

    RData* get_rdata(STATE);

    void set_rdata(RData* rdata) {
      set_data(rdata, eRData);
    }

    bool rtypeddata_p() const {
      return type() == eRData;
    }

    RTypedData* get_rtypeddata(STATE);

    void set_rtypeddata(RTypedData* rtypeddata) {
      set_data(rtypeddata, eRData);
    }

    bool rfloat_p() const {
      return type() == eRFloat;
    }

    RFloat* get_rfloat(STATE);

    void set_rfloat(RFloat* rfloat) {
      set_data(rfloat, eRFloat);
    }

    bool rio_p() const {
      return type() == eRIO;
    }

    RIO* get_rio(STATE);
    IO* get_io(STATE);
    bool rio_close(STATE);

    void set_rio(RIO* rio) {
      set_data(rio, eRIO);
    }

    bool rfile_p() const {
      return type() == eRFile;
    }

    RFile* get_rfile(STATE);

    void set_rfile(RFile* rfile) {
      set_data(rfile, eRFile);
    }
  };

  struct MemoryLock {
    std::mutex lock;
    uintptr_t thread_id;
    uintptr_t locked_count;

    MemoryLock(uintptr_t id, uintptr_t count)
      : lock()
      , thread_id(id)
      , locked_count(count)
    {
      lock.lock();
    }

    ~MemoryLock() {
      if(locked_count > 0) lock.unlock();
    }

    bool locked_p(STATE);
    bool lock_owned_p(STATE);
    bool try_lock(STATE);
    void unlock(STATE);
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

    uintptr_t type() const {
      return word & type_mask();
    }

    uintptr_t ptr_mask() const {
      return ~type_mask();
    }

    bool unset_p() const {
      return type() == eUnsetWord;
    }

    bool object_id_entry_p() const {
      return type() == eObjectID;
    }

    uintptr_t get_object_id() const {
      if(object_id_entry_p()) {
        return reinterpret_cast<uintptr_t>(word >> value_shift());
      }

      return 0;
    }

    void set_object_id(uintptr_t id) {
      word = (id << value_shift()) | eObjectID;
    }

    bool handle_entry_p() const {
      return type() == eHandle;
    }

    MemoryHandle* get_handle() const {
      if(handle_entry_p()) {
        return reinterpret_cast<MemoryHandle*>(word & ptr_mask());
      }

      return nullptr;
    }

    void set_handle(MemoryHandle* handle) {
      word = reinterpret_cast<uintptr_t>(handle) | eHandle;
    }

    bool reference_entry_p() const {
      return type() == eRefCount;
    }

    uintptr_t get_referenced() const {
      if(reference_entry_p()) {
        return reinterpret_cast<uintptr_t>(word >> value_shift());
      }

      return 0;
    }

    void set_referenced(uintptr_t refcount) {
      word = (refcount << value_shift()) | eRefCount;
    }

    bool lock_p() const {
      return type() == eLock;
    }

    MemoryLock* get_lock() const {
      if(lock_p()) {
        return reinterpret_cast<MemoryLock*>(word & ptr_mask());
      }

      return nullptr;
    }

    void set_lock(MemoryLock* lock) {
      word = reinterpret_cast<uintptr_t>(lock) | eLock;
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
        } else if(MemoryLock* lock = words[i].get_lock()) {
          delete lock;
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

    static ExtendedHeader* create_object_id(const MemoryFlags h, uintptr_t id) {
       ExtendedHeader* nh = create(h);
       nh->words[0].set_object_id(id);

       return nh;
    }

    static ExtendedHeader* create_object_id(
        const MemoryFlags h, const ExtendedHeader* eh, uintptr_t id)
    {
      ExtendedHeader* nh = create(h, eh);
      nh->words[eh->size()].set_object_id(id);

      return nh;
    }

    static ExtendedHeader* create_handle(
        const MemoryFlags h, MemoryHandle* handle, Object* object)
    {
      ExtendedHeader* nh = create(h);

      nh->words[0].set_handle(handle);

      return nh;
    }

    static ExtendedHeader* create_handle(
        const MemoryFlags h, ExtendedHeader* eh, MemoryHandle* handle, Object* object)
    {
      ExtendedHeader* nh = create(h, eh);

      nh->words[eh->size()].set_handle(handle);

      return nh;
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

    static ExtendedHeader* create_lock(STATE, const MemoryFlags h, unsigned int count);
    static ExtendedHeader* create_lock(STATE, const MemoryFlags h,
        const ExtendedHeader* eh, unsigned int count);

    static ExtendedHeader* replace_lock(STATE, const MemoryFlags h,
        const ExtendedHeader* eh, unsigned int count);

    void track_memory_header(STATE);

    // We must not run the destructor because the pointers are still valid.
    void delete_zombie_header() {
      delete[] reinterpret_cast<uintptr_t*>(this);
    }

    void delete_header() {
      this->~ExtendedHeader();
      delete_zombie_header();
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

    uintptr_t get_object_id() const {
      for(int i = 0; i < size(); i++) {
        if(uintptr_t id = words[i].get_object_id()) {
          return id;
        }
      }

      return 0;
    }

    void set_object_id(uintptr_t id) {
      for(int i = 0; i < size(); i++) {
        if(words[i].object_id_entry_p()) {
          words[i].set_object_id(id);
        }
      }
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
        if(words[i].reference_entry_p()) {
          words[i].set_referenced(refcount);
        }
      }
    }

    bool lock_extended_p() const {
      return locked_count_field.get(header) == locked_count_field.max();
    }

    MemoryLock* get_lock() const {
      for(int i = 0; i < size(); i++) {
        if(MemoryLock* lock = words[i].get_lock()) {
          return lock;
        }
      }

      return nullptr;
    }

    bool finalizer_p() const {
      return finalizer_field.get(header);
    }

    bool marked_p(unsigned int mark) const {
      return marked_field.get(header) == mark;
    }

    void unsynchronized_set_zombie() {
      ExtendedHeaderWord unset;

      for(int i = 0; i < size(); i++) {
        words[i] = unset;
      }

      header = zombie_field.set(header);
    }

    bool zombie_p() const {
      return zombie_field.get(header);
    }
  };

  struct MemoryHeader {
    std::atomic<MemoryFlags> header;

    static std::atomic<uintptr_t> object_id_counter;

    static void bootstrap(STATE);

    static size_t align(size_t bytes) {
      return (bytes + (sizeof(MemoryHeader*) - 1)) & ~(sizeof(MemoryHeader*) - 1);
    }

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
      return locked_count_field.max() - 1;
    }

    bool reference_p() const {
      return __REFERENCE_P__(this);
    }

    bool fixnum_p() const {
      return __FIXNUM_P__(this);
    }

    bool integer_p() const;

    bool symbol_p() const {
      return __SYMBOL_P__(this);
    }

    static unsigned int lock_extended() {
      return locked_count_field.max();
    }

    ExtendedHeader* extended_header() const {
      return const_cast<ExtendedHeader*>(
          reinterpret_cast<const ExtendedHeader*>(
            header.load(std::memory_order_acquire) & ~0x7L));
    }

    MemoryFlags extended_flags(ExtendedHeader* h) const {
      MemoryFlags f = reinterpret_cast<MemoryFlags>(h);

      return reinterpret_cast<MemoryFlags>(extended_field.set(f));
    }

    bool extended_header_p() const {
      return extended_field.get(header.load(std::memory_order_acquire));
    }

    MemoryHeaderBits header_bits() const;

    bool lock_extended_p() const {
      if(extended_header_p()) {
        return extended_header()->lock_extended_p();
      } else {
        return locked_count_field.get(
            header.load(std::memory_order_acquire)) == lock_extended();
      }
    }

    unsigned int lock_owner() const {
      if(extended_header_p()) {
        ExtendedHeader* hh = extended_header();

        if(hh->lock_extended_p()) {
          return hh->get_lock()->thread_id;
        } else if(locked_count_field.get(hh->header) > 0) {
          return thread_id_field.get(hh->header);
        } else {
          return 0;
        }
      } else {
        if(locked_count_field.get(header.load(std::memory_order_acquire)) > 0) {
          return thread_id_field.get(header.load(std::memory_order_acquire));
        } else {
          return 0;
        }
      }
    }

    unsigned int lock_count() const {
      if(extended_header_p()) {
        ExtendedHeader* hh = extended_header();

        if(hh->lock_extended_p()) {
          return hh->get_lock()->locked_count;
        } else {
          return locked_count_field.get(hh->header);
        }
      } else {
        return locked_count_field.get(header.load(std::memory_order_acquire));
      }
    }

    uintptr_t get(const HeaderField field) const {
      if(extended_header_p()) {
        return field.get(extended_header()->header);
      } else {
        return field.get(header.load(std::memory_order_acquire));
      }
    }

    void unsynchronized_set(const HeaderField field, unsigned int value) {
      if(extended_header_p()) {
        ExtendedHeader* hh = extended_header();
        hh->header = field.set(hh->header, value);
      } else {
        MemoryFlags h = header.load(std::memory_order_acquire);
        header.store(field.set(h, value), std::memory_order_release);
      }
    }

    void set(STATE, const HeaderField field, unsigned int value) {
      while(true) {
        MemoryFlags h = header.load(std::memory_order_acquire);

        if(extended_header_p()) {
          ExtendedHeader* hh = extended_header();
          ExtendedHeader* eh = ExtendedHeader::create_copy(field.set(hh->header, value), hh);
          MemoryFlags nh = extended_flags(eh);

          if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
            hh->unsynchronized_set_zombie();

            eh->track_memory_header(state);

            return;
          }

          eh->delete_zombie_header();
        } else {
          MemoryFlags nh = field.set(h, value);

          if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
            return;
          }
        }
      }
    }

    void unsynchronized_set(const HeaderField field) {
      if(extended_header_p()) {
        ExtendedHeader* hh = extended_header();
        hh->header = field.set(hh->header);
      } else {
        MemoryFlags h = header.load(std::memory_order_acquire);
        header.store(field.set(h), std::memory_order_release);
      }
    }

    void set(STATE, const HeaderField field) {
      while(true) {
        MemoryFlags h = header.load(std::memory_order_acquire);

        if(extended_header_p()) {
          ExtendedHeader* hh = extended_header();
          ExtendedHeader* eh = ExtendedHeader::create_copy(field.set(hh->header), hh);
          MemoryFlags nh = extended_flags(eh);

          if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
            hh->unsynchronized_set_zombie();

            eh->track_memory_header(state);

            return;
          }

          eh->delete_zombie_header();
        } else {
          MemoryFlags nh = field.set(h);

          if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
            return;
          }
        }
      }
    }

    void unsynchronized_unset(const HeaderField field) {
      if(extended_header_p()) {
        ExtendedHeader* hh = extended_header();
        hh->header = field.unset(hh->header);
      } else {
        MemoryFlags h = header.load(std::memory_order_acquire);

        header.store(field.unset(h), std::memory_order_release);
      }
    }

    void unset(STATE, const HeaderField field) {
      while(true) {
        MemoryFlags h = header.load(std::memory_order_acquire);

        if(extended_header_p()) {
          ExtendedHeader* hh = extended_header();
          ExtendedHeader* eh = ExtendedHeader::create_copy(field.unset(hh->header), hh);
          MemoryFlags nh = extended_flags(eh);

          if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
            hh->unsynchronized_set_zombie();

            eh->track_memory_header(state);

            return;
          }

          eh->delete_zombie_header();
        } else {
          MemoryFlags nh = field.unset(h);

          if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
            return;
          }
        }
      }
    }

    bool forwarded_p() const {
      return forwarded_field.get(header.load(std::memory_order_acquire));
    }

    void set_forwarded(MemoryHeader* address) {
      forwarded_field.set(header.load(std::memory_order_acquire));
    }

    Object* forward() const {
      return reinterpret_cast<Object*>(header.load(std::memory_order_acquire) & ~0x7L);
    }

    int thread_id() const {
      return get(thread_id_field);
    }

    void set_thread_id(STATE, uintptr_t id) {
      set(state, thread_id_field, id);
    }

    MemoryRegion region() const {
      return static_cast<MemoryRegion>(get(region_field));
    }

    void region(STATE, MemoryRegion region) {
      set(state, region_field, region);
    }

    bool thread_region_p() const {
      return static_cast<MemoryRegion>(get(region_field)) == eThreadRegion;
    }

    bool first_region_p() const {
      return static_cast<MemoryRegion>(get(region_field)) == eFirstRegion;
    }

    bool second_region_p() const {
      return static_cast<MemoryRegion>(get(region_field)) == eSecondRegion;
    }

    bool third_region_p() const {
      return static_cast<MemoryRegion>(get(region_field)) == eThirdRegion;
    }

    unsigned int referenced_count() const {
      if(extended_header_p()) {
        ExtendedHeader* eh = extended_header();
        uintptr_t refcount = referenced_field.get(eh->header);

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
        return referenced_field.get(header.load(std::memory_order_acquire));
      }
    }

    bool pinned_p() const {
      return (type_id() == ByteArrayType) && get(pinned_field);
    }

    void set_pinned(STATE) {
      set(state, pinned_field);
    }

    void unset_pinned(STATE) {
      unset(state, pinned_field);
    }

    bool weakref_p() const {
      return get(weakref_field);
    }

    void set_weakref(STATE) {
      if(!weakref_p()) {
        set(state, weakref_field);
        track_weakref(state);
      }
    }

    bool finalizer_p() const {
      return get(finalizer_field);
    }

    void set_finalizer(STATE) {
      set(state, finalizer_field);
    }

    void unset_finalizer(STATE) {
      unset(state, finalizer_field);
    }

    bool remembered_p() const {
      return get(remembered_field);
    }

    void set_remembered(STATE) {
      // TODO: MemoryHeader append to references if unset
      set(state, remembered_field);
    }

    void unset_remembered(STATE) {
      unset(state, remembered_field);
    }

    unsigned int visited() const {
      return get(visited_field);
    }

    bool visited_p(unsigned int flag) const {
      return visited() == flag;
    }

    void set_visited(STATE, unsigned int flag) {
      set(state, visited_field, flag);
    }

    unsigned int marked() const {
      return get(marked_field);
    }

    bool marked_p(unsigned int mark) const {
      return marked() == mark;
    }

    void unsynchronized_set_marked(unsigned int mark) {
      unsynchronized_set(marked_field, mark);
    }

    void set_marked(STATE, unsigned int mark) {
      set(state, marked_field, mark);
    }

    bool scanned_p() const {
      return get(scanned_field);
    }

    void unsynchronized_set_scanned() {
      unsynchronized_set(scanned_field);
    };

    void set_scanned(STATE) {
      set(state, scanned_field);
    }

    void unsynchronized_unset_scanned() {
      unsynchronized_unset(scanned_field);
    }

    void unset_scanned(STATE) {
      unset(state, scanned_field);
    }

    object_type type_id() const {
      return static_cast<object_type>(get(type_id_field));
    }

    bool type_p(object_type type) const {
      return static_cast<object_type>(get(type_id_field)) == type;
    }

    bool data_p() const {
      return get(data_field);
    }

    bool object_p() const {
      return !data_p();
    }

    bool handle_p() const {
      return __HANDLE_P__(this);
    }

    bool frozen_p() const {
      return get(frozen_field);
    }

    void set_frozen(STATE) {
      set(state, frozen_field);
    }

    void unset_frozen(STATE) {
      unset(state, frozen_field);
    }

    bool tainted_p() const {
      return get(tainted_field);
    }

    void set_tainted(STATE) {
      set(state, tainted_field);
    }

    void unset_tainted(STATE) {
      unset(state, tainted_field);
    }

    uintptr_t object_id() const {
      if(extended_header_p()) {
        ExtendedHeader* hh = extended_header();

        uintptr_t oid = object_id_field.get(hh->header);

        if(oid > 0 && oid < max_object_id()) {
          return oid;
        } else {
         return hh->get_object_id();
        }
      } else {
        return object_id_field.get(header.load(std::memory_order_acquire));
      }
    }

    unsigned int type_specific() const {
      return get(type_specific_field);
    }

    void set_type_specific(STATE, unsigned int value) {
      set(state, type_specific_field, value);
    }

    bool memory_handle_p() const {
      if(extended_header_p()) {
        return extended_header()->get_handle() != nullptr;
      } else {
        return false;
      }
    }

    // Operations on a managed object.

    void lock(STATE);
    bool try_lock(STATE);
    bool locked_p(STATE);
    bool lock_owned_p(STATE);
    void unlock(STATE);
    void unset_lock(STATE);

    uintptr_t assign_object_id(STATE) {
      uintptr_t id = object_id_counter.fetch_add(1);

      while(true) {
        MemoryFlags h = header.load(std::memory_order_acquire);

        if(uintptr_t oid = object_id()) return oid;

        if(extended_header_p()) {
          ExtendedHeader* eh;
          ExtendedHeader* hh = extended_header();

          if(id < max_object_id()) {
            eh = ExtendedHeader::create_copy(object_id_field.set(hh->header, id), hh);
          } else {
            eh = ExtendedHeader::create_object_id(
                object_id_field.set(hh->header, max_object_id()), hh, id);
          }

          MemoryFlags nh = extended_flags(eh);

          if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
            hh->unsynchronized_set_zombie();

            eh->track_memory_header(state);

            return id;
          }

          eh->delete_zombie_header();
        } else {
          if(id < max_object_id()) {
            MemoryFlags nh = object_id_field.set(h, id);

            if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
              return id;
            }
          } else {
            ExtendedHeader* eh = ExtendedHeader::create_object_id(
                object_id_field.set(h, max_object_id()), id);
            MemoryFlags nh = extended_flags(eh);

            if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
              eh->track_memory_header(state);

              return id;
            }

            eh->delete_zombie_header();
          }
        }
      }
    }

    void track_reference(STATE);
    void track_weakref(STATE);

    MemoryHandle* memory_handle(STATE, Object* object) {
      MemoryHandle* handle = nullptr;

      while(true) {
        MemoryFlags h = header.load(std::memory_order_acquire);
        ExtendedHeader* hh = nullptr;
        ExtendedHeader* eh;

        if(extended_header_p()) {
          hh = extended_header();

          if(MemoryHandle* handle = hh->get_handle()) {
            return handle;
          }
        }

        if(!handle) {
          handle = new MemoryHandle(object);
        }

        if(hh) {
          eh = ExtendedHeader::create_handle(hh->header, hh, handle, object);
        } else {
          eh = ExtendedHeader::create_handle(h, handle, object);
        }

        MemoryFlags nh = extended_flags(eh);

        if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
          if(hh) hh->unsynchronized_set_zombie();

          eh->track_memory_header(state);

          return handle;
        }

        eh->delete_zombie_header();
      }
    }

    int add_reference(STATE) {
      if(!reference_p()) return 0;

      while(true) {
        MemoryFlags h = header.load(std::memory_order_acquire);

        if(extended_header_p()) {
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

          if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
            hh->unsynchronized_set_zombie();

            eh->track_memory_header(state);

            if(refcount == 0) track_reference(state);
            return refcount + 1;
          }

          eh->delete_zombie_header();
        } else {
          unsigned int refcount = referenced_field.get(h);

          if(refcount < max_referenced()) {
            MemoryFlags nh = referenced_field.set(h, refcount + 1);

            if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
              if(refcount == 0) track_reference(state);
              return refcount + 1;
            }
          } else {
            ExtendedHeader* eh = ExtendedHeader::create_referenced(h, refcount + 1);

            MemoryFlags nh = extended_flags(eh);

            if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
              if(refcount == 0) track_reference(state);
              return refcount + 1;
            }

            eh->delete_zombie_header();
          }
        }
      }
    }

    int sub_reference(STATE) {
      if(!reference_p()) return 0;

      while(true) {
        MemoryFlags h = header.load(std::memory_order_acquire);

        if(extended_header_p()) {
          ExtendedHeader* eh;
          ExtendedHeader* hh = extended_header();
          uintptr_t refcount = referenced_field.get(hh->header);

          if(refcount > 0) {
            if(refcount < max_referenced()) {
              eh = ExtendedHeader::create_copy(
                  referenced_field.set(hh->header, refcount - 1), hh);
            } else {
              if(uintptr_t rc = hh->get_referenced()) {
                refcount = rc;

                eh = ExtendedHeader::create_copy(hh->header, hh);
                eh->set_referenced(refcount - 1);
              } else {
                eh = ExtendedHeader::create_referenced(hh->header, hh, refcount - 1);
              }
            }
          } else {
            return refcount;
          }

          MemoryFlags nh = extended_flags(eh);

          if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
            hh->unsynchronized_set_zombie();

            eh->track_memory_header(state);

            return refcount - 1;
          }

          eh->delete_zombie_header();
        } else {
          unsigned int refcount = referenced_field.get(h);

          if(refcount > 0) {
            if(refcount < max_referenced()) {
              MemoryFlags nh = referenced_field.set(h, refcount - 1);

              if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
                return refcount - 1;
              }
            } else {
              ExtendedHeader* eh = ExtendedHeader::create_referenced(h, refcount - 1);

              MemoryFlags nh = extended_flags(eh);

              if(header.compare_exchange_strong(h, nh, std::memory_order_release)) {
                return refcount - 1;
              }

              eh->delete_zombie_header();
            }
          } else {
            return refcount;
          }
        }
      }
    }

    void write_barrier(STATE, MemoryHeader* value);
  };

  class DataHeader : public MemoryHeader {
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

    static size_t bytes_to_fields(size_t bytes) {
      return (bytes - sizeof(DataHeader)) / sizeof(Object*);
    }

    // Called when the memory size is not static.
    size_t compute_size_in_bytes(STATE) const;

    size_t size_in_bytes(STATE) const {
      size_t size = TypeInfo::instance_sizes[type_id()];
      if(size != 0) {
        return size;
      } else {
        return compute_size_in_bytes(state);
      }
    }

    void initialize_fields(intptr_t bytes) {
      void** fields = __body__;
      intptr_t field_count = bytes_to_fields(bytes);

      for(intptr_t i = 0; i < field_count; i++) {
        fields[i] = cNil;
      }
    }

    size_t body_in_bytes(STATE) const {
      return size_in_bytes(state) - sizeof(DataHeader);
    }

    void** pointer_to_body() {
      return __body__;
    }
  };

  class ObjectHeader : public MemoryHeader {

#ifdef RBX_TEST
  public:
#else
  protected:
#endif

    attr_accessor(klass, Class);
    attr_accessor(ivars, Object);

  private:
    void* __body__[0];

  public:
    ObjectHeader() = delete;
    ObjectHeader(const ObjectHeader&) = delete;
    ObjectHeader& operator= (const ObjectHeader&) = delete;

    ~ObjectHeader() = delete;

  public:

    static size_t bytes_to_fields(size_t bytes) {
      return (bytes - sizeof(ObjectHeader)) / sizeof(Object*);
    }

  public:

    void initialize_copy(STATE, Object* other);

    /* Copies the body of +other+ into +this+ */
    void copy_body(STATE, Object* other);

    /* Used to make an exact state copy of +this+ into +other* */
    void initialize_full_state(STATE, Object* other, unsigned int age);

    /* Clear the body of the object, by setting each field to cNil */
    void initialize_fields(intptr_t bytes) {
      void** fields = __body__;
      intptr_t field_count = bytes_to_fields(bytes);

      for(intptr_t i = 0; i < field_count; i++) {
        fields[i] = cNil;
      }
    }

    void** pointer_to_body() {
      return __body__;
    }

    /* It's the slow case, should be called only if there's no cached
     * instance size. */
    size_t compute_size_in_bytes(STATE) const;

    /* The whole point of this is inlining */
    size_t size_in_bytes(STATE) const {
      size_t size = TypeInfo::instance_sizes[type_id()];
      if(size != 0) {
        return size;
      } else {
        return compute_size_in_bytes(state);
      }
    }

    size_t body_in_bytes(STATE) const {
      return size_in_bytes(state) - sizeof(ObjectHeader);
    }

    Class* reference_class() const {
      return _klass_;
    }

    void wait(STATE);

    bool equal_p(const ObjectHeader* other) const {
      return (this == other) || NIL_EQUAL_P(this, other);
    }

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

    void validate() const {
      assert(!reference_p() || (type_id() > InvalidType && type_id() < LastObjectType));
    }

    friend class TypeInfo;
    friend class Memory;
  };

  namespace memory {
    void write_barrier(STATE, ObjectHeader* object, Fixnum* value);
    void write_barrier(STATE, ObjectHeader* object, Symbol* value);
    void write_barrier(STATE, ObjectHeader* object, ObjectHeader* value);
    void write_barrier(STATE, ObjectHeader* object, Class* value);
  }
}

#endif
