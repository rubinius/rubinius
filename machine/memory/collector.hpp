#ifndef RBX_MEMORY_COLLECTOR_HPP
#define RBX_MEMORY_COLLECTOR_HPP

#include "machine_threads.hpp"

#include "memory/root.hpp"

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>

namespace rubinius {
  class VM;
  class State;
  class Memory;
  class Object;
  struct CallFrame;

  namespace memory {
    class ImmixGC;

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
      virtual void finalize(STATE) = 0;
      virtual void mark(ImmixGC* gc) = 0;
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
      void mark(ImmixGC* gc);
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
      void mark(ImmixGC* gc);
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
      void mark(ImmixGC* gc);
      bool match_p(STATE, Object* object, Object* finalizer);
    };

    typedef std::list<FinalizerObject*> FinalizerObjects;

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
          : collector_(state->shared().collector())
        {
          collector_->inhibit_collection();
        }

        ~Inhibit() {
          collector_->allow_collection();
        }
      };

      class Worker : public MachineThread {
        FinalizerObjects& process_list_;

        std::mutex& list_mutex_;
        std::condition_variable& list_condition_;

      public:
        Worker(STATE, FinalizerObjects& list, std::mutex& lk, std::condition_variable& cond);
        ~Worker() { }

        void initialize(STATE);
        void stop(STATE);
        void wakeup(STATE);
        void after_fork_child(STATE);
        void run(STATE);
      };

    private:
      Worker* worker_;

      FinalizerObjects live_list_;
      FinalizerObjects process_list_;

      std::mutex list_mutex_;
      std::condition_variable list_condition_;

      std::atomic<bool> finishing_;

      std::atomic<int> inhibit_collection_;
      std::atomic<bool> collect_requested_;

    public:
      Collector(STATE);
      virtual ~Collector();

      std::mutex& list_mutex() {
        return list_mutex_;
      }

      std::condition_variable& list_condition() {
        return list_condition_;
      }

      void finish(STATE);
      void dispose(STATE);

      void native_finalizer(STATE, Object* obj, FinalizerFunction func);
      void extension_finalizer(STATE, Object* obj, FinalizerFunction func);
      void managed_finalizer(STATE, Object* obj, Object* finalizer);

      void add_finalizer(STATE, FinalizerObject* obj);

      void gc_scan(ImmixGC* gc, Memory* memory);

      void stop(STATE);
      void wakeup(STATE);

      void inhibit_collection() {
        ++inhibit_collection_;
      }

      void allow_collection() {
        --inhibit_collection_;
        if(inhibit_collection_ < 0) inhibit_collection_ = 0;
      }

      bool collect_p() {
        return inhibit_collection_ == 0;
      }

      void collect_requested(STATE);
      void collect(STATE);
    };
  }
}

#endif
