#ifndef RBX_OBJECTMEMORY_H
#define RBX_OBJECTMEMORY_H

#include "defines.hpp"
#include "memory/header.hpp"

#include "type_info.hpp"
#include "object_position.hpp"
#include "metrics.hpp"
#include "configuration.hpp"

#include "class/class.hpp"
#include "class/module.hpp"
#include "class/object.hpp"

#include "memory/code_manager.hpp"
#include "memory/finalizer.hpp"
#include "memory/write_barrier.hpp"
#include "memory/immix_collector.hpp"

#include "util/atomic.hpp"
#include "util/thread.hpp"

#include "shared_state.hpp"

class TestMemory; // So we can friend it properly
class TestVM; // So we can friend it properly

namespace rubinius {
  struct CallFrame;
  class Configuration;
  class Integer;
  class Thread;

  namespace memory {
    class FinalizerThread;
    class GCData;
    class ImmixGC;
    class ImmixMarker;
    class InflatedHeaders;
    class MarkSweepGC;
    class Slab;
  }

  namespace capi {
    class Handle;
    class Handles;
    class GlobalHandle;
  }

  /**
   * Memory is the primary API that the rest of the VM uses to interact
   * with actions such as allocating objects, storing data in objects, and
   * performing garbage collection.
   *
   * It is currently split among 3 generations:
   *   - BakerGC:     handles young objects
   *   - ImmixGC:     handles mature objects
   *   - MarkSweepGC: handles large objects
   *
   * Memory also manages the memory used for CodeResources, which are
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

  class Memory : public memory::WriteBarrier {
  private:
    utilities::thread::SpinLock allocation_lock_;
    utilities::thread::SpinLock inflation_lock_;

    /// BakerGC used for the young generation
    /* BakerGC* young_; */

    /// MarkSweepGC used for the large object store
    memory::MarkSweepGC* mark_sweep_;

    /// ImmixGC used for the mature generation
    memory::ImmixGC* immix_;

    /// ImmixMarker thread used for the mature generation
    memory::ImmixMarker* immix_marker_;

    /// Storage for all InflatedHeader instances.
    memory::InflatedHeaders* inflated_headers_;

    /// Storage for C-API handle allocator, cached C-API handles
    /// and global handle locations.
    capi::Handles* capi_handles_;
    std::list<capi::Handle*> cached_capi_handles_;
    std::list<capi::GlobalHandle*> global_capi_handle_locations_;

    /// Garbage collector for CodeResource objects.
    memory::CodeManager code_manager_;

    /// The number of GC cycles that have run
    unsigned int cycle_;

    /// The current mark value used when marking objects.
    unsigned int mark_;

    /// Flag controlling whether garbage collections are allowed
    bool allow_gc_;
    /// Flag set when concurrent mature mark is requested
    bool mature_mark_concurrent_;
    /// Flag set when a mature GC is already in progress
    bool mature_gc_in_progress_;
    /// Flag set when requesting a young gen resize

    /// Size of slabs to be allocated to threads for lockless thread-local
    /// allocations.
    size_t slab_size_;

    /// Flag indicating that a Memory condition exists
    bool interrupt_flag_;

    /// Flag indicating whether a young collection should be performed soon
    bool collect_young_flag_;

    /// Flag indicating whether a full collection should be performed soon
    bool collect_full_flag_;

    /// Mutex used to manage lock contention
    utilities::thread::Mutex contention_lock_;

    /// Condition variable used to manage lock contention
    utilities::thread::Condition contention_var_;

    SharedState& shared_;

  public:
    VM* vm_;
    /// Counter used for issuing object ids when #object_id is called on a
    /// Ruby object.
    size_t last_object_id;
    size_t last_snapshot_id;
    TypeInfo* type_info[(int)LastObjectType];

    /* Config variables */
    size_t large_object_threshold;

  public:
    static void memory_error(STATE);

    void set_vm(VM* vm) {
      vm_ = vm;
    }

    VM* vm() {
      return vm_;
    }

    SharedState& shared() {
      return shared_;
    }

    Memory* memory() {
      return this;
    }

    unsigned int cycle() {
      return cycle_;
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

    void schedule_young_collection(VM* vm, metrics::metric& counter) {
      counter++;

      // Don't trigger GC if currently prohibited so we don't thrash checking.
      if(can_gc()) {
        interrupt_flag_ = collect_young_flag_ = true;
        shared_.thread_nexus()->set_stop();
      }
    }

    void schedule_full_collection(const char* trigger, metrics::metric& counter) {
      counter++;
      schedule_full_collection(trigger);
    }

    void schedule_full_collection(const char* trigger) {
      // Don't trigger if already triggered.
      if(collect_full_flag_) return;

      // Don't trigger GC if currently prohibited so we don't thrash checking.
      if(shared_.config.memory_collection_log.value) {
        logger::write("memory: full collection: trigger: %s", trigger);
      }

      if(can_gc()) {
        interrupt_flag_ = collect_full_flag_ = true;
        shared_.thread_nexus()->set_stop();
      } else if(shared_.config.memory_collection_log.value) {
        logger::write("memory: collection: disabled");
      }
    }

    memory::FinalizerThread* finalizer() const {
      return shared_.finalizer();
    }

    memory::InflatedHeaders* inflated_headers() const {
      return inflated_headers_;
    }

    capi::Handles* capi_handles() const {
      return capi_handles_;
    }

    memory::ImmixMarker* immix_marker() const {
      return immix_marker_;
    }

    void set_immix_marker(memory::ImmixMarker* immix_marker) {
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
    Memory(VM* state, SharedState& shared);
    ~Memory();

    void after_fork_child(STATE);

    inline void write_barrier(ObjectHeader* target, Fixnum* val) {
      /* No-op */
    }

    inline void write_barrier(ObjectHeader* target, Symbol* val) {
      /* No-op */
    }

    inline void write_barrier(ObjectHeader* target, ObjectHeader* val) {
      memory::WriteBarrier::write_barrier(target, val, mark_);
    }

    inline void write_barrier(ObjectHeader* target, Class* val) {
      memory::WriteBarrier::write_barrier(target, reinterpret_cast<Object*>(val), mark_);
    }

    // Object must be created in Immix or large object space.
    Object* new_object(STATE, native_int bytes);

    /* Allocate a new object in any space that will accommodate it based on
     * the following priority:
     *  1. SLAB (state-local allocation buffer, no locking needed)
     *  2. immix space (mature generation, lock needed)
     *  3. LOS (large object space, lock needed)
     *
     * The resulting object is UNINITIALIZED. The caller is responsible for
     * initializing all reference fields other than _klass_ and _ivars_.
     */
    Object* new_object(STATE, Class* klass, native_int bytes, object_type type) {
      // TODO: GC
    // allocate:
      Object* obj = 0; /* state->vm()->local_slab().allocate(bytes).as<Object>();

      if(likely(obj)) {
        state->vm()->metrics().memory.young_objects++;
        state->vm()->metrics().memory.young_bytes += bytes;

        obj->init_header(YoungObjectZone, type);

        goto set_klass;
      }

      if(state->vm()->local_slab().empty_p()) {
        if(refill_slab(state, state->vm()->local_slab())) {
          goto allocate;
        } else {
          schedule_young_collection(state->vm(), state->vm()->metrics().gc.young_set);
       }
      }
      */

      if(likely(obj = new_object(state, bytes))) goto set_type;

      Memory::memory_error(state);
      return NULL;

    set_type:
      obj->set_obj_type(type);

    // set_klass:
      obj->klass(state, klass);
      obj->ivars(cNil);

      obj->set_cycle(cycle_);

      return obj;
    }

    /* Allocate a new, pinned, object in any space that will accommodate it
     * based on the following priority:
     *  1. immix space (mature generation, lock needed)
     *  2. LOS (large object space, lock needed)
     *
     * The resulting object is UNINITIALIZED. The caller is responsible for
     * initializing all reference fields other than _klass_ and _ivars_.
     */
    Object* new_object_pinned(STATE, Class* klass, native_int bytes, object_type type) {
      Object* obj = new_object(state, bytes);

      if(unlikely(!obj)) {
        Memory::memory_error(state);
        return NULL;
      }

      obj->set_pinned();
      obj->set_obj_type(type);

      obj->klass(state, klass);
      obj->ivars(cNil);

      obj->set_cycle(cycle_);

      return obj;
    }

    template <class T>
      T* new_object(STATE, Class* klass, native_int bytes, object_type type) {
        T* obj = new_object(state, klass, bytes, type);
        T::initialize(state, obj, bytes, type);

        return obj;
      }

    template <class T>
      T* new_object(STATE, Class *klass) {
        T* obj = static_cast<T*>(new_object(state, klass, sizeof(T), T::type));
        T::initialize(state, obj);

        return obj;
      }

    template <class T>
      T* new_object(STATE, Class *klass, native_int bytes) {
        return static_cast<T*>(new_object(state, klass, bytes, T::type));
      }

    template <class T>
      T* new_variable_object(STATE, Class *klass) {
        T* obj = static_cast<T*>(new_object(
              state, klass, TypeInfo::instance_sizes[T::type], T::type));
        T::initialize(state, obj);

        return obj;
      }

    template <class T>
      T* new_bytes(STATE, Class* klass, native_int bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = static_cast<T*>(new_object(state, klass, bytes, T::type));

        obj->full_size(bytes);

        return obj;
      }

    template <class T>
      T* new_fields(STATE, Class* klass, native_int fields) {
        native_int bytes = sizeof(T) + (fields * sizeof(Object*));
        T* obj = static_cast<T*>(new_object(state, klass, bytes, T::type));

        obj->full_size(bytes);

        return obj;
      }

    template <class T>
      T* new_object_pinned(STATE, Class *klass) {
        T* obj = static_cast<T*>(new_object_pinned(state, klass, sizeof(T), T::type));
        T::initialize(state, obj);

        return obj;
      }

    template <class T>
      T* new_bytes_pinned(STATE, Class* klass, native_int bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = static_cast<T*>(new_object_pinned(state, klass, bytes, T::type));

        obj->full_size(bytes);

        return obj;
      }

    template <class T>
      T* new_fields_pinned(STATE, Class* klass, native_int fields) {
        native_int bytes = sizeof(T) + (fields * sizeof(Object*));
        T* obj = static_cast<T*>(new_object_pinned(state, klass, bytes, T::type));

        obj->full_size(bytes);

        return obj;
      }

    // New classes.
    template <class T>
      Class* new_class(STATE, Class* super) {
        T* klass =
          static_cast<T*>(new_object(state, G(klass), sizeof(T), T::type));
        T::initialize(state, klass, super);

        return klass;
      }

    template <class T>
      Class* new_class(STATE, Module* under, const char* name) {
        return new_class<T>(state, G(object), under, name);
      }

    template <class T>
      T* new_class(STATE, Class* super, Module* under, const char* name) {
        return new_class<T>(state, super, under, state->symbol(name));
      }

    template <class T>
      T* new_class(STATE, Class* super, Module* under, Symbol* name) {
        T* klass =
          static_cast<T*>(new_object(state, G(klass), sizeof(T), T::type));
        T::initialize(state, klass, super, under, name);

        return klass;
      }

    template <class S, class R>
      Class* new_class(STATE, Module* under, const char* name) {
        return new_class<S, R>(state, G(object), under, name);
      }

    template <class S, class R>
      Class* new_class(STATE, Class* super, const char* name) {
        return new_class<S, R>(state, super, G(object), name);
      }

    template <class S, class R>
      Class* new_class(STATE, const char* name) {
        return new_class<S, R>(state, G(object), G(object), name);
      }

    template <class S, class R>
      S* new_class(STATE, Class* super, Module* under, const char* name) {
        return new_class<S, R>(state, super, under, state->symbol(name));
      }

    template <class S, class R>
      S* new_class(STATE, Class* super, Module* under, Symbol* name) {
        S* klass =
          static_cast<S*>(new_object(state, G(klass), sizeof(S), S::type));
        S::initialize(state, klass, super, under, name, R::type);

        return klass;
      }

    // New modules.
    template <class T>
      T* new_module(STATE) {
        return new_module<T>(state, G(module));
      }

    template <class T>
      T* new_module(STATE, Class* super) {
        return state->memory()->new_object<T>(state, super);
      }

    template <class T>
      T* new_module(STATE, Module* under, const char* name) {
        return new_module<T>(state, G(module), under, name);
      }

    template <class T>
      T* new_module(STATE, Class* super, Module* under, const char* name) {
        T *mod = static_cast<T*>(state->memory()->new_object(
              state, super, sizeof(T), T::type));
        T::initialize(state, mod, under, name);

        return mod;
      }

    template <class T>
      T* new_module(STATE, const char* name) {
        return new_module<T>(state, G(module), G(object), name);
      }

    TypeInfo* find_type_info(Object* obj);
    Object* promote_object(Object* obj);

    bool refill_slab(STATE, memory::Slab& slab);

    void assign_object_id(STATE, Object* obj);
    bool inflate_lock_count_overflow(STATE, ObjectHeader* obj, int count);
    LockStatus contend_for_lock(STATE, ObjectHeader* obj, size_t us, bool interrupt);
    void release_contention(STATE);
    bool inflate_and_lock(STATE, ObjectHeader* obj);
    bool inflate_for_contention(STATE, ObjectHeader* obj);

    bool valid_object_p(Object* obj);
    void add_type_info(TypeInfo* ti);

    void add_code_resource(STATE, memory::CodeResource* cr);
    void memstats();

    void validate_handles(capi::Handles* handles);
    void prune_handles(capi::Handles* handles, std::list<capi::Handle*>* cached, /* BakerGC */ void* young);

    ObjectPosition validate_object(Object* obj);

    void collect(STATE) {
      if(can_gc()) {
        collect_young_flag_ = true;
        collect_full_flag_ = true;
        interrupt_flag_ = true;
        state->vm()->thread_nexus()->set_stop();
        state->vm()->checkpoint(state);
      }
    }

    void collect_maybe(STATE);

    void native_finalizer(STATE, Object* obj, memory::FinalizerFunction func) {
      if(memory::FinalizerThread* f = this->finalizer()) {
        f->native_finalizer(state, obj, func);
      }
    }

    void extension_finalizer(STATE, Object* obj, memory::FinalizerFunction func) {
      if(memory::FinalizerThread* f = this->finalizer()) {
        f->extension_finalizer(state, obj, func);
      }
    }

    void managed_finalizer(STATE, Object* obj, Object* finalizer) {
      if(memory::FinalizerThread* f = this->finalizer()) {
        f->managed_finalizer(state, obj, finalizer);
      }
    }

    InflatedHeader* inflate_header(STATE, ObjectHeader* obj);
    void inflate_for_id(STATE, ObjectHeader* obj, uint32_t id);
    void inflate_for_handle(STATE, ObjectHeader* obj, capi::Handle* handle);

    // TODO: generalize when fixing safe points.
    String* new_string_certain(STATE, Class* klass);

    bool mature_gc_in_progress() {
      return mature_gc_in_progress_;
    }

    void clear_mature_mark_in_progress() {
      mature_gc_in_progress_ = false;
    }

    memory::MarkStack& mature_mark_stack();

    void set_interrupt() {
      interrupt_flag_ = true;
      atomic::memory_barrier();
    }

    void reset_interrupt() {
      interrupt_flag_ = false;
    }

    bool& interrupt_p() {
      return interrupt_flag_;
    }

    bool& collect_young_p() {
      return collect_young_flag_;
    }

    bool& collect_full_p() {
      return collect_full_flag_;
    }

    void collect_young(STATE, memory::GCData* data);
    void collect_full(STATE);
    void collect_full_restart(STATE, memory::GCData* data);
    void collect_full_finish(STATE, memory::GCData* data);

  public:
    friend class ::TestMemory;
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
      Memory* om_;

    public:
      GCInhibit(Memory* om)
        : om_(om)
      {
        om->inhibit_gc();
      }

      GCInhibit(STATE)
        : om_(state->memory())
      {
        om_->inhibit_gc();
      }

      ~GCInhibit() {
        om_->allow_gc();
      }
    };
  };
};

#endif
