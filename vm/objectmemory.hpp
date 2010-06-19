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

class TestObjectMemory; // So we can friend it properly

namespace rubinius {

  class Object;

  /* ObjectMemory is the primary API that the rest of the VM uses to interact
   * with actions such as allocating objects, storing data in objects, and
   * perform garbage collection.
   *
   * It is current split between 2 generations, the BakerGC, which handles
   * the young objects, and the MarkSweepGC, which handles the mature.
   *
   * Basic tasks:
   *
   * Allocate an object of a given class and number of fields.
   *   If the object is large, it's put to start in the mature space,
   *   otherwise in the young space.
   *
   * Detection of memory condition requiring collection of both generations
   *   independently.
   *
   */

  struct CallFrame;
  class GCData;
  class Configuration;
  class BakerGC;
  class MarkSweepGC;
  class ImmixGC;
  class InflatedHeaders;

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

  class ObjectMemory : public gc::WriteBarrier {
    BakerGC* young_;
    MarkSweepGC* mark_sweep_;

    ImmixGC* immix_;
    InflatedHeaders* inflated_headers_;

    unsigned int mark_;
    CodeManager code_manager_;
    std::list<FinalizeObject> finalize_;
    std::list<FinalizeObject*> to_finalize_;
    bool allow_gc_;

    std::list<gc::WriteBarrier*> aux_barriers_;

  public:
    bool collect_young_now;
    bool collect_mature_now;

    STATE;
    size_t last_object_id;
    size_t last_snapshot_id;
    TypeInfo* type_info[(int)LastObjectType];

    /* Config variables */
    size_t large_object_threshold;

    /* Stats */
    size_t objects_allocated;
    size_t bytes_allocated;

    size_t young_collections;
    size_t full_collections;

    size_t young_collection_time;
    size_t full_collection_time;

  public:
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

    void add_aux_barrier(gc::WriteBarrier* wb) {
      aux_barriers_.push_back(wb);
    }

    void del_aux_barrier(gc::WriteBarrier* wb) {
      aux_barriers_.remove(wb);
    }

    std::list<gc::WriteBarrier*>& aux_barriers() {
      return aux_barriers_;
    }

  public:
    ObjectMemory(STATE, Configuration& config);
    ~ObjectMemory();

    Object* new_object_typed(Class* cls, size_t bytes, object_type type);
    Object* new_object_typed_mature(Class* cls, size_t bytes, object_type type);
    Object* new_object_typed_enduring(Class* cls, size_t bytes, object_type type);

    Object* new_object_fast(Class* cls, size_t bytes, object_type type);

    template <class T>
      T* new_object_bytes(Class* cls, size_t& bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = reinterpret_cast<T*>(new_object_typed(cls, bytes, T::type));

        return obj;
      }

    template <class T>
      T* new_object_bytes_mature(Class* cls, size_t& bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = reinterpret_cast<T*>(new_object_typed_mature(cls, bytes, T::type));

        return obj;
      }

    template <class T>
      T* new_object_variable(Class* cls, size_t fields, size_t& bytes) {
        bytes = sizeof(T) + (fields * sizeof(Object*));
        return reinterpret_cast<T*>(new_object_typed(cls, bytes, T::type));
      }

    template <class T>
      T* new_object_enduring(Class* cls) {
        return reinterpret_cast<T*>(
            new_object_typed_enduring(cls, sizeof(T), T::type));
      }

    TypeInfo* find_type_info(Object* obj);
    void set_young_lifetime(size_t age);
    void collect_young(GCData& data, YoungCollectStats* stats = 0);
    void collect_mature(GCData& data);
    Object* promote_object(Object* obj);
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

    void needs_finalization(Object* obj, FinalizerFunction func = 0);
    void run_finalizers(STATE);

    void find_referers(Object* obj, ObjectArray& result);
    void print_references(Object* obj);

    void snapshot();
    void print_new_since_snapshot();

    size_t& loe_usage();
    size_t& immix_usage();
    size_t& code_usage();

    InflatedHeader* inflate_header(ObjectHeader* obj);

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
