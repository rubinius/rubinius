#ifndef RBX_OBJECTMEMORY_H
#define RBX_OBJECTMEMORY_H

#include "defines.hpp"
#include "memory/header.hpp"

#include "type_info.hpp"
#include "configuration.hpp"
#include "globals.hpp"
#include "spinlock.hpp"
#include "symbol_table.hpp"

#include "class/class.hpp"
#include "class/module.hpp"
#include "class/object.hpp"

#include "memory/code_manager.hpp"
#include "memory/main_heap.hpp"

#include "diagnostics.hpp"

#include "thread_state.hpp"

#include <atomic>
#include <functional>
#include <mutex>

class TestMemory; // So we can friend it properly
class TestVM; // So we can friend it properly

namespace rubinius {
  struct CallFrame;
  struct MemoryHeader;

  class Configuration;
  class Integer;
  class Thread;

  namespace memory {
    class Collector;
    class MainHeap;

    class CAPITracer {
      ThreadState* state_;
      std::function<void (STATE, Object**)> tracer_;

    public:
      CAPITracer(STATE, std::function<void (STATE, Object**)> f)
        : state_(state)
        , tracer_(f)
      {
      }
      virtual ~CAPITracer() { }

      Object* call(Object* obj) {
        Object* object = obj;
        tracer_(state_, &object);
        return object == obj ? nullptr : object;
      }
    };
  }

  /**
   * Memory is the primary API that the rest of the VM uses to interact
   * with actions such as allocating objects, storing data in objects, and
   * performing garbage collection.
   *
   * It is currently split among 3 generations:
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

  class Memory {
  private:
    locks::spinlock_mutex allocation_lock_;

    /// Garbage collector for CodeResource objects.
    memory::CodeManager code_manager_;

    memory::MainHeap* main_heap_;

    /// The number of GC cycles that have run
    unsigned int cycle_;

    /// The current mark value used when marking objects.
    unsigned int mark_;

    unsigned int visit_mark_;

    std::atomic<uint64_t> class_count_;
    std::atomic<int> global_serial_;

    std::recursive_mutex codedb_lock_;

    locks::spinlock_mutex type_info_lock_;
    locks::spinlock_mutex code_resource_lock_;

  public:
    /// Counter used for issuing object ids when #object_id is called on a
    /// Ruby object.
    size_t last_object_id;
    size_t last_snapshot_id;
    TypeInfo* type_info[(int)LastObjectType];

    /* Config variables */
    size_t large_object_threshold;

    Globals globals;
    SymbolTable symbols;

  public:
    Memory(STATE, Configuration* configuration);
    virtual ~Memory();

    static void memory_error(STATE);

    void collect_cycle() {
      cycle_++;
    }

    locks::spinlock_mutex& allocation_lock() {
      return allocation_lock_;
    }

    int inc_class_count() {
      return ++class_count_;
    }

    int global_serial() const {
      return global_serial_;
    }

    int inc_global_serial() {
      return ++global_serial_;
    }

    std::recursive_mutex& codedb_lock() {
      return codedb_lock_;
    }

    locks::spinlock_mutex& type_info_lock() {
      return type_info_lock_;
    }

    locks::spinlock_mutex& code_resource_lock() {
      return code_resource_lock_;
    }

    memory::CodeManager& code_manager() {
      return code_manager_;
    }

    memory::MainHeap* main_heap() {
      return main_heap_;
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
      mark_ ^= 0x3;
    }

    unsigned int visit_mark() const {
      return visit_mark_;
    }

    void rotate_visit_mark() {
      visit_mark_ ^= 0x3;
    }

    void after_fork_child(STATE);

    // Object must be created in Immix or large object space.
    Object* new_object(STATE, intptr_t bytes, object_type type);
    Object* new_object_pinned(STATE, intptr_t bytes, object_type type);

    /* Allocate a new object in any space that will accommodate it based on
     * the following priority:
     *  1. SLAB (state-local allocation buffer, no locking needed)
     *  2. immix space (mature generation, lock needed)
     *  3. LOS (large object space, lock needed)
     *
     * The resulting object is UNINITIALIZED. The caller is responsible for
     * initializing all reference fields other than _klass_ and _ivars_.
     */
    Object* new_object(STATE, Class* klass, intptr_t bytes, object_type type) {
      Object* obj = state->allocate_object(state, bytes, type);

      obj->klass(state, klass);
      obj->ivars(cNil);

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
    Object* new_object_pinned(STATE, Class* klass, intptr_t bytes, object_type type) {
      Object* obj = new_object_pinned(state, bytes, type);

      obj->klass(state, klass);
      obj->ivars(cNil);

      return obj;
    }

    template <class T>
      T* new_object(STATE, Class* klass, intptr_t bytes, object_type type) {
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
      T* new_object(STATE, Class *klass, intptr_t bytes) {
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
      T* new_bytes(STATE, Class* klass, intptr_t bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = static_cast<T*>(new_object(state, klass, bytes, T::type));

        obj->full_size(bytes);

        return obj;
      }

    template <class T>
      T* new_fields(STATE, Class* klass, intptr_t fields) {
        intptr_t bytes = sizeof(T) + (fields * sizeof(Object*));
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
      T* new_bytes_pinned(STATE, Class* klass, intptr_t bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        T* obj = static_cast<T*>(new_object_pinned(state, klass, bytes, T::type));

        obj->full_size(bytes);

        return obj;
      }

    template <class T>
      T* new_fields_pinned(STATE, Class* klass, intptr_t fields) {
        intptr_t bytes = sizeof(T) + (fields * sizeof(Object*));
        T* obj = static_cast<T*>(new_object_pinned(state, klass, bytes, T::type));

        obj->full_size(bytes);

        return obj;
      }

    template <class T>
      T* new_object_unmanaged(STATE, Class* klass, intptr_t* mem) {
        T* obj = reinterpret_cast<T*>(mem);

        MemoryHeader::initialize(obj, state->thread_id(), eThreadRegion, T::type, false);

        obj->klass(state, klass);
        obj->ivars(cNil);

        T::initialize(state, obj);

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

    TypeInfo* find_type(int type);
    TypeInfo* find_type_info(Object* obj);

    bool valid_object_p(Object* obj);
    void add_type_info(Memory* memory, TypeInfo* ti);

    void add_code_resource(STATE, memory::CodeResource* cr);

    // TODO: GC
    // ObjectPosition validate_object(Object* obj);

  public:
    friend class ::TestMemory;
    friend class ::TestVM;
  };
};

#endif
