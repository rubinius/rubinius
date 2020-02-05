#ifndef RBX_MEMORY_COLLECTOR_HPP
#define RBX_MEMORY_COLLECTOR_HPP

#include "machine_thread.hpp"
#include "logger.hpp"
#include "thread_state.hpp"

#include "memory/header.hpp"
#include "memory/root.hpp"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <unordered_set>

namespace rubinius {
  class ThreadState;
  class Memory;
  class Object;
  struct CallFrame;

  namespace memory {
    class MemoryTracer;

    typedef void (*FinalizerFunction)(STATE, Object*);

    class FinalizerObject {
      Object* object_;

    public:
      FinalizerObject(STATE, Object* object)
        : object_(object)
      { }
      virtual ~FinalizerObject() { }

      Object* object() const {
        return object_;
      }

      void object(Object* obj) {
        object_ = obj;
      }

      virtual void dispose(STATE) = 0;
      virtual void finalize(STATE);
      virtual void mark(STATE, MemoryTracer* tracer) = 0;
      virtual bool match_p(STATE, Object* object, Object* finalizer) = 0;
    };

    class NativeFinalizer : public FinalizerObject {
      FinalizerFunction finalizer_;

    public:
      NativeFinalizer(STATE, Object* object, FinalizerFunction finalizer)
        : FinalizerObject(state, object)
        , finalizer_(finalizer)
      { }

      void dispose(STATE);
      void finalize(STATE);
      void mark(STATE, MemoryTracer* tracer);
      bool match_p(STATE, Object* object, Object* finalizer) { return false; }
    };

    class ExtensionFinalizer : public FinalizerObject {
      FinalizerFunction finalizer_;

    public:
      ExtensionFinalizer(STATE, Object* object, FinalizerFunction finalizer)
        : FinalizerObject(state, object)
        , finalizer_(finalizer)
      { }

      void dispose(STATE);
      void finalize(STATE);
      void mark(STATE, MemoryTracer* tracer);
      bool match_p(STATE, Object* object, Object* finalizer) { return false; }
    };

    class ManagedFinalizer : public FinalizerObject {
      Object* finalizer_;

    public:
      ManagedFinalizer(STATE, Object* object, Object* finalizer)
        : FinalizerObject(state, object)
        , finalizer_(finalizer)
      { }

      void dispose(STATE);
      void finalize(STATE);
      void mark(STATE, MemoryTracer* tracer);
      bool match_p(STATE, Object* object, Object* finalizer);
    };

    typedef std::unordered_set<MemoryHeader*> References;
    typedef std::unordered_set<MemoryHeader*> Weakrefs;

    typedef std::list<ExtendedHeader*> MemoryHeaders;
    typedef std::list<FinalizerObject*> Finalizers;

    class Collector {
    public:
      /**
       * Object used to prevent garbage collections from running for a short
       * period while the memory is scanned, e.g. to find referrers to an
       * object or take a snapshot of the heap. Typically, an instance of this
       * class is created at the start of a method that requires the heap to
       * be stable. When the method ends, the object goes out of scope and is
       * destroyed, re-enabling garbage collections.
       */
      class Inhibit {
        Collector* collector_;

      public:
        Inhibit(STATE)
          : collector_(state->collector())
        {
          collector_->inhibit_collection();
        }

        ~Inhibit() {
          collector_->allow_collection();
        }
      };

      class Worker : public MachineThread {
        Collector* collector_;

        Finalizers& process_list_;

        std::mutex& list_mutex_;
        std::condition_variable& list_condition_;

      public:
        Worker(STATE, Collector* collector,
            Finalizers& list, std::mutex& lk, std::condition_variable& cond);
        ~Worker() { }

        void initialize(STATE);
        void stop(STATE);
        void wakeup(STATE);
        void run(STATE);
      };

    private:
      Worker* worker_;

      Finalizers finalizer_list_;
      Finalizers process_list_;

      MemoryHeaders memory_headers_list_;
      locks::spinlock_mutex memory_headers_lock_;

      References references_set_;
      locks::spinlock_mutex references_lock_;

      Weakrefs weakrefs_set_;
      locks::spinlock_mutex weakrefs_lock_;

      std::mutex collector_mutex_;
      std::mutex list_mutex_;
      std::condition_variable list_condition_;

      std::atomic<bool> finishing_;

      std::atomic<int> inhibit_collection_;
      std::atomic<bool> collect_requested_;

    public:
      Collector();
      virtual ~Collector();

      std::mutex& list_mutex() {
        return list_mutex_;
      }

      std::condition_variable& list_condition() {
        return list_condition_;
      }

      MemoryHeaders& memory_headers() {
        return memory_headers_list_;
      }

      References& references() {
        return references_set_;
      }

      Weakrefs& weakrefs() {
        return weakrefs_set_;
      }

      void add_memory_header(ExtendedHeader* header) {
        std::lock_guard<locks::spinlock_mutex> guard(memory_headers_lock_);

        memory_headers_list_.push_back(header);
      }

      void add_reference(MemoryHeader* header) {
        if(header->reference_p()) {
          std::lock_guard<locks::spinlock_mutex> guard(references_lock_);

          references_set_.insert(header);
        }
      }

      void add_weakref(MemoryHeader* header) {
        if(header->reference_p()) {
          std::lock_guard<locks::spinlock_mutex> guard(weakrefs_lock_);

          weakrefs_set_.insert(header);
        }
      }

      void finish(STATE);
      void dispose(STATE);

      void native_finalizer(STATE, Object* obj, FinalizerFunction func);
      void extension_finalizer(STATE, Object* obj, FinalizerFunction func);
      void managed_finalizer(STATE, Object* obj, Object* finalizer);

      void add_finalizer(STATE, FinalizerObject* obj);

      void trace_finalizers(STATE, MemoryTracer* tracer);

      void start(STATE);
      void stop(STATE);

      void notify() {
        list_condition_.notify_one();
      }

      void worker_exited(STATE) {
        worker_ = nullptr;
      }

      void inhibit_collection() {
        ++inhibit_collection_;
      }

      void allow_collection() {
        --inhibit_collection_;
        if(inhibit_collection_ < 0) inhibit_collection_ = 0;
      }

      bool collectable_p() {
        return inhibit_collection_ == 0;
      }

      bool collect_requested_p() {
        return collect_requested_.load(std::memory_order_acquire);
      }

      void collect_requested(STATE, const char* reason) {
        logger::info(reason);

        if(collectable_p()) {
          {
            std::lock_guard<std::mutex> guard(collector_mutex_);

            collect_requested_ = true;
          }

          notify();
        }
      }

      void collect_completed(STATE) {
        std::lock_guard<std::mutex> guard(collector_mutex_);

        collect_requested_ = false;
      }

      void collect(STATE);
      void stop_for_collection(STATE, std::function<void ()> process);

      void after_fork_child(STATE);
    };
  }
}

#endif
