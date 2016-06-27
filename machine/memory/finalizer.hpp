#ifndef RBX_GC_FINALIZE_HPP
#define RBX_GC_FINALIZE_HPP

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

    class FinalizerThread : public MachineThread {
      class Synchronization {
        std::mutex list_mutex_;
        std::condition_variable list_condition_;

      public:
        Synchronization()
          : list_mutex_()
          , list_condition_()
        { }

        std::mutex& list_mutex() {
          return list_mutex_;
        }

        std::condition_variable& list_condition() {
          return list_condition_;
        }
      };

      FinalizerObjects live_list_;
      FinalizerObjects process_list_;

      Synchronization* synchronization_;

      std::atomic<bool> finishing_;

    public:
      FinalizerThread(STATE);
      virtual ~FinalizerThread();

      std::mutex& list_mutex() {
        return synchronization_->list_mutex();
      }

      std::condition_variable& list_condition() {
        return synchronization_->list_condition();
      }

      void finish(STATE);
      void dispose(STATE);

      void native_finalizer(STATE, Object* obj, FinalizerFunction func);
      void extension_finalizer(STATE, Object* obj, FinalizerFunction func);
      void managed_finalizer(STATE, Object* obj, Object* finalizer);

      void add_finalizer(STATE, FinalizerObject* obj);

      void gc_scan(ImmixGC* gc, Memory* memory);

      void initialize(STATE);
      void run(STATE);
      void stop(STATE);
      void wakeup(STATE);
      void after_fork_child(STATE);

      void cleanup();
    };
  }
}

#endif
