#ifndef RBX_GC_FINALIZE_HPP
#define RBX_GC_FINALIZE_HPP

#include "internal_threads.hpp"

#include "memory/root.hpp"

#include <list>

namespace rubinius {
  class VM;
  class State;
  struct CallFrame;
  class Object;

namespace memory {
  typedef void (*FinalizerFunction)(STATE, Object*);

  struct FinalizeObject {
  public:
    enum FinalizeKind {
      eManaged,
      eUnmanaged
    };

    enum FinalizationStatus {
      eLive,
      eQueued,
      eRubyFinalized,
      eNativeFinalized,
      eReleased
    };

  public:
    FinalizeObject()
      : object(NULL)
      , kind(eManaged)
      , status(eLive)
      , finalizer(0)
      , ruby_finalizer(0)
    {}

    Object* object;
    FinalizeKind kind;
    FinalizationStatus status;
    FinalizerFunction finalizer;
    Object* ruby_finalizer;

    void queued() {
      status = eQueued;
    }

    bool queued_p() const {
      return status == eQueued;
    }
  };

  typedef std::list<FinalizeObject> FinalizeObjects;
  typedef std::list<FinalizeObjects*> FinalizeObjectsList;

  class FinalizerThread : public InternalThread {
  public:
    class iterator {
      FinalizerThread* handler_;
      FinalizeObjects* current_list_;
      FinalizeObjects::iterator end_;
      FinalizeObjects::iterator current_;
      FinalizeObjectsList::iterator lists_iterator_;

    public:
      iterator(FinalizerThread* fh);
      ~iterator() {}

      void next(bool live);
      bool end();
      FinalizeObject& current() { return *current_; }
    };

    friend class iterator;

    enum ProcessItemKind {
      eRuby,
      eNative,
      eRelease
    };

  private:
    FinalizeObjectsList* lists_;
    FinalizeObjects* live_list_;
    FinalizeObjects* process_list_;
    FinalizeObjects::iterator process_item_;
    FinalizerThread::iterator* iterator_;
    ProcessItemKind process_item_kind_;
    utilities::thread::Mutex live_guard_;
    utilities::thread::Mutex worker_lock_;
    utilities::thread::Condition worker_cond_;
    utilities::thread::Mutex supervisor_lock_;
    utilities::thread::Condition supervisor_cond_;
    bool finishing_;

  public:

    FinalizerThread(STATE);
    virtual ~FinalizerThread();

    void finalize(STATE);
    void first_process_item();
    void next_process_item();
    void finish(STATE);

    void record(Object* obj, FinalizerFunction func, FinalizeObject::FinalizeKind kind);
    void set_ruby_finalizer(Object* obj, Object* finalizer);

    void queue_objects(STATE);

    void start_collection(STATE);
    void finish_collection(STATE);

    FinalizerThread::iterator& begin();

    void worker_signal();
    void worker_wait();
    void supervisor_signal();
    void supervisor_wait();

    void initialize(STATE);
    void run(STATE);
    void stop(STATE);
    void wakeup(STATE);
  };
}
}

#endif
