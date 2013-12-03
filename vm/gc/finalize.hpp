#ifndef RBX_GC_FINALIZE_HPP
#define RBX_GC_FINALIZE_HPP

#include "lock.hpp"
#include "auxiliary_threads.hpp"

#include "gc/finalize.hpp"
#include "gc/root.hpp"

#include <list>

namespace rubinius {
  class VM;
  class State;
  struct CallFrame;
  class Object;
  class Thread;

  typedef void (*FinalizerFunction)(STATE, Object*);

  struct FinalizeObject {
  public:
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
      , status(eLive)
      , finalizer(0)
      , ruby_finalizer(0)
    {}

    Object* object;
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

  class FinalizerHandler : public AuxiliaryThread, public Lockable {
  public:
    class iterator {
      FinalizerHandler* handler_;
      FinalizeObjects* current_list_;
      FinalizeObjects::iterator end_;
      FinalizeObjects::iterator current_;
      FinalizeObjectsList::iterator lists_iterator_;

    public:
      iterator(FinalizerHandler* fh);
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
    SharedState& shared_;
    VM* self_;

    TypedRoot<Thread*> thread_;
    FinalizeObjectsList* lists_;
    FinalizeObjects* live_list_;
    FinalizeObjects* process_list_;
    FinalizeObjects::iterator process_item_;
    FinalizerHandler::iterator* iterator_;
    ProcessItemKind process_item_kind_;
    utilities::thread::Mutex live_guard_;
    utilities::thread::Mutex worker_lock_;
    utilities::thread::Condition worker_cond_;
    utilities::thread::Condition pause_cond_;
    utilities::thread::Mutex supervisor_lock_;
    utilities::thread::Condition supervisor_cond_;
    bool paused_;
    bool exit_;
    bool finishing_;

  public:

    FinalizerHandler(STATE);
    virtual ~FinalizerHandler();

    void run(STATE);
    void perform(STATE);
    void finalize(STATE);
    void first_process_item();
    void next_process_item();
    void finish(STATE, GCToken gct);

    void record(Object* obj, FinalizerFunction func);
    void set_ruby_finalizer(Object* obj, Object* finalizer);

    void queue_objects();

    void start_collection(STATE);
    void finish_collection(STATE);

    FinalizerHandler::iterator& begin();

    void worker_signal();
    void worker_wait();
    void supervisor_signal();
    void supervisor_wait();

    void start_thread(STATE);
    void stop_thread(STATE);

    void shutdown(STATE);
    void before_exec(STATE);
    void after_exec(STATE);
    void before_fork(STATE);
    void after_fork_parent(STATE);
    void after_fork_child(STATE);
  };
}

#endif
