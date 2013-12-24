#ifndef RBX_OBJECTMEMORY_H
#define RBX_OBJECTMEMORY_H

#include "prelude.hpp"
#include "type_info.hpp"

#include "object_position.hpp"

#include "oop.hpp"
#include "gc/code_manager.hpp"
#include "gc/finalize.hpp"
#include "gc/write_barrier.hpp"

#include "util/immix.hpp"
#include "util/thread.hpp"
#include "lock.hpp"

#include "shared_state.hpp"

class TestObjectMemory; // So we can friend it properly
class TestVM; // So we can friend it properly

namespace rubinius {

  class Object;
  class Integer;

  struct CallFrame;
  class GCData;
  class Configuration;
  class BakerGC;
  class MarkSweepGC;
  class ImmixGC;
  class InflatedHeaders;
  class Thread;
  class FinalizerHandler;
  class ImmixMarker;

  namespace gc {
    class Slab;
  }

  namespace capi {
    class Handle;
    class Handles;
    class GlobalHandle;
  }

  struct YoungCollectStats {
    int bytes_copied;
    double percentage_used;
    int promoted_objects;
    int lifetime;
    int excess_objects;

    YoungCollectStats()
      : bytes_copied(0)
      , percentage_used(0.0)
      , promoted_objects(0)
      , lifetime(0)
      , excess_objects(0)
    {}
  };


  struct GCStats {

    atomic::integer young_objects_allocated;
    atomic::integer young_bytes_allocated;
    atomic::integer promoted_objects_allocated;
    atomic::integer promoted_bytes_allocated;
    atomic::integer mature_objects_allocated;
    atomic::integer mature_bytes_allocated;

    atomic::integer young_collection_count;
    atomic::integer full_collection_count;

    atomic::integer total_young_collection_time;
    atomic::integer total_full_stop_collection_time;
    atomic::integer total_full_concurrent_collection_time;
    atomic::integer last_young_collection_time;
    atomic::integer last_full_stop_collection_time;
    atomic::integer last_full_concurrent_collection_time;

    void young_object_allocated(uint64_t size) {
      young_objects_allocated.inc();
      young_bytes_allocated.add(size);
    }

    void promoted_object_allocated(uint64_t size) {
      promoted_objects_allocated.inc();
      promoted_bytes_allocated.add(size);
    }

    void mature_object_allocated(uint64_t size) {
      mature_objects_allocated.inc();
      mature_bytes_allocated.add(size);
    }

    void slab_allocated(uint64_t count, uint64_t size) {
      young_objects_allocated.add(count);
      young_bytes_allocated.add(size);
    }

    GCStats()
      : young_objects_allocated(0)
      , young_bytes_allocated(0)
      , promoted_objects_allocated(0)
      , promoted_bytes_allocated(0)
      , mature_objects_allocated(0)
      , mature_bytes_allocated(0)
      , young_collection_count(0)
      , full_collection_count(0)
      , total_young_collection_time(0)
      , total_full_stop_collection_time(0)
      , total_full_concurrent_collection_time(0)
      , last_young_collection_time(0)
      , last_full_stop_collection_time(0)
      , last_full_concurrent_collection_time(0)
    {}
  };


  /**
   * ObjectMemory is the primary API that the rest of the VM uses to interact
   * with actions such as allocating objects, storing data in objects, and
   * performing garbage collection.
   *
   * It is currently split between 3 generations, the BakerGC, which handles
   * the young objects, the ImmixGC which handles mature objects, and the
   * MarkSweepGC, which handles large objects.
   *
   * ObjectMemory also manages the memory used for CodeResources, which are
   * internal objects used for executing Ruby code. This includes MachineCode,
   * various JIT classes, and FFI data.
   *
   * Basic tasks:
   * - Allocate an object of a given class and number of fields. If the object
   *   is large, it's allocated in the large object space, otherwise in the
   *   young space.
   * - Detection of memory condition requiring collection of the young and
   *   mautre generations independently.
   */

  class ObjectMemory : public gc::WriteBarrier, public Lockable {

    utilities::thread::SpinLock allocation_lock_;
    utilities::thread::SpinLock inflation_lock_;

    /// BakerGC used for the young generation
    BakerGC* young_;

    /// MarkSweepGC used for the large object store
    MarkSweepGC* mark_sweep_;

    /// ImmixGC used for the mature generation
    ImmixGC* immix_;

    /// ImmixMarker thread used for the mature generation
    ImmixMarker* immix_marker_;

    /// Storage for all InflatedHeader instances.
    InflatedHeaders* inflated_headers_;

    /// Storage for C-API handle allocator, cached C-API handles
    /// and global handle locations.
    capi::Handles* capi_handles_;
    std::list<capi::Handle*> cached_capi_handles_;
    std::list<capi::GlobalHandle*> global_capi_handle_locations_;

    /// Garbage collector for CodeResource objects.
    CodeManager code_manager_;

    /// The current mark value used when marking objects.
    unsigned int mark_;

    unsigned int young_gc_while_marking_;
    /// Flag controlling whether garbage collections are allowed
    bool allow_gc_;
    /// Flag set when concurrent mature mark is requested
    bool mature_mark_concurrent_;
    /// Flag set when a mature GC is already in progress
    bool mature_gc_in_progress_;
    /// Flag set when requesting a young gen resize
    bool young_gc_resize_;

    /// Size of slabs to be allocated to threads for lockless thread-local
    /// allocations.
    size_t slab_size_;

    /// Mutex used to manage lock contention
    utilities::thread::Mutex contention_lock_;

    /// Condition variable used to manage lock contention
    utilities::thread::Condition contention_var_;

    SharedState& shared_;

  public:
    /// Flag indicating whether a young collection should be performed soon
    bool collect_young_now;

    /// Flag indicating whether a full collection should be performed soon
    bool collect_mature_now;

    VM* root_state_;
    /// Counter used for issuing object ids when #object_id is called on a
    /// Ruby object.
    size_t last_object_id;
    size_t last_snapshot_id;
    TypeInfo* type_info[(int)LastObjectType];

    /* Config variables */
    size_t large_object_threshold;
    size_t young_max_bytes;
    int young_autotune_factor;
    bool young_autotune_size;

    GCStats gc_stats;

  public:
    VM* state() {
      return root_state_;
    }

    unsigned int mark() const {
      return mark_;
    }

    const unsigned int* mark_address() const {
      return &mark_;
    }

    void rotate_mark() {
      mark_ = (mark_ == 2 ? 4 : 2);
    }

    bool can_gc() const {
      return allow_gc_;
    }

    void allow_gc() {
      allow_gc_ = true;
    }

    void inhibit_gc() {
      allow_gc_ = false;
    }

    FinalizerHandler* finalizer_handler() const {
      return shared_.finalizer_handler();
    }

    InflatedHeaders* inflated_headers() const {
      return inflated_headers_;
    }

    capi::Handles* capi_handles() const {
      return capi_handles_;
    }

    ImmixMarker* immix_marker() const {
      return immix_marker_;
    }

    void set_immix_marker(ImmixMarker* immix_marker) {
      immix_marker_ = immix_marker;
    }

    capi::Handle* add_capi_handle(STATE, Object* obj);
    void make_capi_handle_cached(State*, capi::Handle* handle);

    std::list<capi::Handle*>* cached_capi_handles() {
      return &cached_capi_handles_;
    }

    std::list<capi::GlobalHandle*>* global_capi_handle_locations() {
      return &global_capi_handle_locations_;
    }

    void add_global_capi_handle_location(STATE, capi::Handle** loc, const char* file, int line);
    void del_global_capi_handle_location(STATE, capi::Handle** loc);

    ObjectArray* weak_refs_set();

  public:
    ObjectMemory(VM* state, Configuration& config);
    ~ObjectMemory();

    void on_fork(STATE);

    Object* new_object_typed_dirty(STATE, Class* cls, size_t bytes, object_type type);
    Object* new_object_typed(STATE, Class* cls, size_t bytes, object_type type);

    Object* new_object_typed_mature_dirty(STATE, Class* cls, size_t bytes, object_type type);
    Object* new_object_typed_mature(STATE, Class* cls, size_t bytes, object_type type);

    Object* new_object_typed_enduring_dirty(STATE, Class* cls, size_t bytes, object_type type);
    Object* new_object_typed_enduring(STATE, Class* cls, size_t bytes, object_type type);

    template <class T>
      T* new_object_bytes_dirty(STATE, Class* cls, size_t& bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = static_cast<T*>(new_object_typed_dirty(state, cls, bytes, T::type));

        return obj;
      }

    template <class T>
      T* new_object_bytes(STATE, Class* cls, size_t& bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = static_cast<T*>(new_object_typed(state, cls, bytes, T::type));

        return obj;
      }

    template <class T>
      T* new_object_bytes_mature_dirty(STATE, Class* cls, size_t& bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = static_cast<T*>(new_object_typed_mature_dirty(state, cls, bytes, T::type));

        return obj;
      }

    template <class T>
      T* new_object_bytes_mature(STATE, Class* cls, size_t& bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = static_cast<T*>(new_object_typed_mature(state, cls, bytes, T::type));

        return obj;
      }

    template <class T>
      T* new_object_variable(STATE, Class* cls, size_t fields, size_t& bytes) {
        bytes = sizeof(T) + (fields * sizeof(Object*));
        return static_cast<T*>(new_object_typed(state, cls, bytes, T::type));
      }

    template <class T>
      T* new_object_enduring(STATE, Class* cls) {
        return static_cast<T*>(
            new_object_typed_enduring(state, cls, sizeof(T), T::type));
      }

    void inline write_barrier(ObjectHeader* target, ObjectHeader* val) {
      gc::WriteBarrier::write_barrier(target, val, mark_);
    }

    TypeInfo* find_type_info(Object* obj);
    void set_young_lifetime(size_t age);
    Object* promote_object(Object* obj);

    bool refill_slab(STATE, gc::Slab& slab);

    void assign_object_id(STATE, Object* obj);
    bool inflate_lock_count_overflow(STATE, ObjectHeader* obj, int count);
    LockStatus contend_for_lock(STATE, GCToken gct, CallFrame* call_frame, ObjectHeader* obj, size_t us, bool interrupt);
    void release_contention(STATE, GCToken gct, CallFrame* call_frame);
    bool inflate_and_lock(STATE, ObjectHeader* obj);
    bool inflate_for_contention(STATE, ObjectHeader* obj);

    bool valid_object_p(Object* obj);
    void debug_marksweep(bool val);
    void add_type_info(TypeInfo* ti);

    void add_code_resource(CodeResource* cr);
    void memstats();

    void validate_handles(capi::Handles* handles);
    void prune_handles(capi::Handles* handles, std::list<capi::Handle*>* cached, BakerGC* young);
    void clear_fiber_marks(std::list<ManagedThread*>* threads);

    ObjectPosition validate_object(Object* obj);
    bool valid_young_object_p(Object* obj);

    size_t young_bytes_allocated() const;
    size_t mature_bytes_allocated() const;
    size_t code_bytes_allocated() const;
    size_t symbol_bytes_allocated() const;
    size_t jit_bytes_allocated() const;

    void collect_maybe(STATE, GCToken gct, CallFrame* call_frame);

    void needs_finalization(Object* obj, FinalizerFunction func);
    void set_ruby_finalizer(Object* obj, Object* finalizer);

    void* young_start();
    void* yound_end();

    size_t& loe_usage();
    size_t& young_usage();
    size_t& immix_usage();
    size_t& code_usage();

    InflatedHeader* inflate_header(STATE, ObjectHeader* obj);
    void inflate_for_id(STATE, ObjectHeader* obj, uint32_t id);
    void inflate_for_handle(STATE, ObjectHeader* obj, capi::Handle* handle);

    /// This only has one use! Don't use it!
    Object* allocate_object_raw(size_t bytes);
    void collect_mature_finish(STATE, GCData* data);
    void wait_for_mature_marker(STATE);
    void clear_mature_mark_in_progress() {
      mature_gc_in_progress_ = false;
    }

    immix::MarkStack& mature_mark_stack();

    void young_autotune();

    void print_young_stats(STATE, GCData* data, YoungCollectStats* stats);
    void print_mature_stats(STATE, GCData* data);

  private:
    Object* allocate_object(size_t bytes);
    Object* allocate_object_mature(size_t bytes);

    void collect_young(STATE, GCData* data, YoungCollectStats* stats = 0);
    void collect_mature(STATE, GCData* data);

  public:
    friend class ::TestObjectMemory;
    friend class ::TestVM;


    /**
     * Object used to prevent garbage collections from running for a short
     * period while the memory is scanned, e.g. to find referrers to an
     * object or take a snapshot of the heap. Typically, an instance of
     * this class is created at the start of a method that requires the
     * heap to be stable. When the method ends, the object goes out of
     * scope and is destroyed, re-enabling garbage collections.
     */

    class GCInhibit {
      ObjectMemory* om_;

    public:
      GCInhibit(ObjectMemory* om)
        : om_(om)
      {
        om->inhibit_gc();
      }

      ~GCInhibit() {
        om_->allow_gc();
      }
    };
  };

#define FREE(obj) free(obj)
#define ALLOC_N(type, size) ((type*)calloc((size), sizeof(type)))
#define ALLOC(t) (t*)XMALLOC(sizeof(t))
#define REALLOC_N(v,t,n) (v)=(t*)realloc((void*)(v), sizeof(t)*n)

#define ALLOCA_N(type, size) ((type*)alloca(sizeof(type) * (size)))
#define ALLOCA(type) ((type*)alloca(sizeof(type)))
};


extern "C" {
  void* XMALLOC(size_t bytes);
  void  XFREE(void* ptr);
  void* XREALLOC(void* ptr, size_t bytes);
  void* XCALLOC(size_t items, size_t bytes);
}

#endif
