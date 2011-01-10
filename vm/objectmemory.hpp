#ifndef RBX_OBJECTMEMORY_H
#define RBX_OBJECTMEMORY_H

#include "prelude.hpp"
#include "type_info.hpp"

#include "object_position.hpp"

#include "call_frame_list.hpp"

#include "builtin/object.hpp"
#include "gc/code_manager.hpp"
#include "gc/finalize.hpp"
#include "gc/write_barrier.hpp"

#include "util/thread.hpp"
#include "lock.hpp"

class TestObjectMemory; // So we can friend it properly

namespace rubinius {

  class Object;

  struct CallFrame;
  class GCData;
  class Configuration;
  class BakerGC;
  class MarkSweepGC;
  class ImmixGC;
  class InflatedHeaders;

  namespace gc {
    class Slab;
  }

  typedef std::vector<Object*> ObjectArray;

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
   * internal objects used for executing Ruby code. This includes VMMethod,
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
    /// BakerGC used for the young generation
    BakerGC* young_;

    /// MarkSweepGC used for the large object store
    MarkSweepGC* mark_sweep_;

    /// ImmixGC used for the mature generation
    ImmixGC* immix_;

    /// Storage for all InflatedHeader instances.
    InflatedHeaders* inflated_headers_;

    unsigned int mark_;

    /// Garbage collector for CodeResource objects.
    CodeManager code_manager_;
    std::list<FinalizeObject> finalize_;
    std::list<FinalizeObject*> to_finalize_;
    bool allow_gc_;

    /// List of additional write-barriers that may hold references to young
    /// objects.
    std::list<gc::WriteBarrier*> aux_barriers_;

    /// Size of slabs to be allocated to threads for lockless thread-local
    /// allocations.
    size_t slab_size_;

    /// True if finalizers are currently being run.
    bool running_finalizers_;

    thread::Condition contention_var_;

  public:
    bool collect_young_now;
    bool collect_mature_now;

    VM* root_state_;
    size_t last_object_id;
    size_t last_snapshot_id;
    TypeInfo* type_info[(int)LastObjectType];

    /* Config variables */
    /// Threshhold size at which an object is considered a large object, and
    /// therefore allocated in the large object space.
    size_t large_object_threshold;

    /* Stats */
    /// Total number of objects allocated to date.
    size_t objects_allocated;

    /// Total number of bytes allocated to date.
    size_t bytes_allocated;

    /// Total number of young generation collections to date.
    size_t young_collections;

    /// Total number of full collections to date.
    size_t full_collections;

    /// Total amount of time spent collecting the young generation to date.
    size_t young_collection_time;

    /// Total amount of time spent performing full collections to date.
    size_t full_collection_time;

  public:
    VM* state() {
      return root_state_;
    }

    unsigned int mark() {
      return mark_;
    }

    void rotate_mark() {
      mark_ = (mark_ == 1 ? 2 : 1);
    }

    std::list<FinalizeObject>& finalize() {
      return finalize_;
    }

    std::list<FinalizeObject*>& to_finalize() {
      return to_finalize_;
    }

    bool can_gc() {
      return allow_gc_;
    }

    void allow_gc() {
      allow_gc_ = true;
    }

    void inhibit_gc() {
      allow_gc_ = false;
    }

    /**
     * Adds an additional write-barrier to the auxilliary write-barriers list.
     */
    void add_aux_barrier(STATE, gc::WriteBarrier* wb) {
      SYNC(state);
      aux_barriers_.push_back(wb);
    }

    /**
     * Removes a write-barrier from the auxilliary wirte-barriers list.
     */
    void del_aux_barrier(STATE, gc::WriteBarrier* wb) {
      SYNC(state);
      aux_barriers_.remove(wb);
    }

    std::list<gc::WriteBarrier*>& aux_barriers() {
      return aux_barriers_;
    }

    bool running_finalizers() {
      return running_finalizers_;
    }

  public:
    ObjectMemory(STATE, Configuration& config);
    ~ObjectMemory();

    Object* new_object_typed(STATE, Class* cls, size_t bytes, object_type type);
    Object* new_object_typed_mature(STATE, Class* cls, size_t bytes, object_type type);
    Object* new_object_typed_enduring(STATE, Class* cls, size_t bytes, object_type type);

    Object* new_object_fast(STATE, Class* cls, size_t bytes, object_type type);

    template <class T>
      T* new_object_bytes(STATE, Class* cls, size_t& bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = reinterpret_cast<T*>(new_object_typed(state, cls, bytes, T::type));

        return obj;
      }

    template <class T>
      T* new_object_bytes_mature(STATE, Class* cls, size_t& bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = reinterpret_cast<T*>(new_object_typed_mature(state, cls, bytes, T::type));

        return obj;
      }

    template <class T>
      T* new_object_variable(STATE, Class* cls, size_t fields, size_t& bytes) {
        bytes = sizeof(T) + (fields * sizeof(Object*));
        return reinterpret_cast<T*>(new_object_typed(state, cls, bytes, T::type));
      }

    template <class T>
      T* new_object_enduring(STATE, Class* cls) {
        return reinterpret_cast<T*>(
            new_object_typed_enduring(state, cls, sizeof(T), T::type));
      }

    TypeInfo* find_type_info(Object* obj);
    void set_young_lifetime(size_t age);
    void collect_young(GCData& data, YoungCollectStats* stats = 0);
    void collect_mature(GCData& data);
    Object* promote_object(Object* obj);

    bool refill_slab(STATE, gc::Slab& slab);

    void assign_object_id(STATE, Object* obj);
    bool inflate_lock_count_overflow(STATE, ObjectHeader* obj, int count);
    LockStatus contend_for_lock(STATE, ObjectHeader* obj, bool* error, size_t us=0);
    void release_contention(STATE);
    bool inflate_and_lock(STATE, ObjectHeader* obj);
    bool inflate_for_contention(STATE, ObjectHeader* obj);

    bool valid_object_p(Object* obj);
    void debug_marksweep(bool val);
    void add_type_info(TypeInfo* ti);

    void add_code_resource(CodeResource* cr);
    void memstats();

    void validate_handles(capi::Handles* handles);
    void prune_handles(capi::Handles* handles, bool check_forwards);

    ObjectPosition validate_object(Object* obj);
    bool valid_young_object_p(Object* obj);

    int mature_bytes_allocated();

    void collect(STATE, CallFrame* call_frame);
    void collect_maybe(STATE, CallFrame* call_frame);

    void needs_finalization(Object* obj, FinalizerFunction func);
    void set_ruby_finalizer(Object* obj, Object* fin);
    void run_finalizers(STATE, CallFrame* call_frame);
    void run_all_finalizers(STATE);

    void find_referers(Object* obj, ObjectArray& result);
    void print_references(Object* obj);

    void* young_start();
    void* yound_end();

    void snapshot();
    void print_new_since_snapshot();

    size_t& loe_usage();
    size_t& immix_usage();
    size_t& code_usage();

    InflatedHeader* inflate_header(STATE, ObjectHeader* obj);
    void inflate_for_id(STATE, ObjectHeader* obj, uint32_t id);

    // This only has one use! Don't use it!
    Object* allocate_object_raw(size_t bytes);

  private:
    Object* allocate_object(size_t bytes);
    Object* allocate_object_mature(size_t bytes);

  public:
    friend class ::TestObjectMemory;


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
