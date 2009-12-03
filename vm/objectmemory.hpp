#ifndef RBX_OBJECTMEMORY_H
#define RBX_OBJECTMEMORY_H

// #include "gc/marksweep.hpp"
// #include "gc/baker.hpp"
// #include "gc/immix.hpp"

#include "prelude.hpp"
#include "type_info.hpp"

#include "object_position.hpp"

#include "call_frame_list.hpp"

#include "builtin/object.hpp"

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

  class CallFrame;
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

  class ObjectMemory {
    BakerGC* young_;
    MarkSweepGC* mark_sweep_;

    ImmixGC* immix_;
    InflatedHeaders* inflated_headers_;

    ObjectArray* remember_set_;
    unsigned int mark_;

  public:
    bool collect_young_now;
    bool collect_mature_now;

    STATE;
    size_t last_object_id;
    TypeInfo* type_info[(int)LastObjectType];

    /* Config variables */
    size_t large_object_threshold;

  public:
    ObjectArray* remember_set() {
      return remember_set_;
    }

    ObjectArray* swap_remember_set();

    unsigned int mark() {
      return mark_;
    }

    void rotate_mark() {
      mark_ = (mark_ == 1 ? 2 : 1);
    }

  public:
    ObjectMemory(STATE, Configuration& config);
    ~ObjectMemory();

    void remember_object(Object* target);
    void unremember_object(Object* target);

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

    void validate_handles(capi::Handles* handles);
    void prune_handles(capi::Handles* handles, bool check_forwards);

    ObjectPosition validate_object(Object* obj);
    bool valid_young_object_p(Object* obj);

    int mature_bytes_allocated();

    InflatedHeader* inflate_header(ObjectHeader* obj);

    void write_barrier(Object* target, Object* val) {
      if(target->remembered_p()) return;
      if(!REFERENCE_P(val)) return;
      if(target->zone() == YoungObjectZone) return;
      if(val->zone() != YoungObjectZone) return;

      remember_object(target);
    }

    // This only has one use! Don't use it!
    Object* allocate_object_raw(size_t bytes);

  private:
    Object* allocate_object(size_t bytes);
    Object* allocate_object_mature(size_t bytes);

  public:
    friend class ::TestObjectMemory;
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
