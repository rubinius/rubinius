#ifndef RBX_GC_IMMIX_MARKER_HPP
#define RBX_GC_IMMIX_MARKER_HPP

#include "lock.hpp"
#include "auxiliary_threads.hpp"

#include "gc/root.hpp"

namespace rubinius {
  class VM;
  class State;
  struct CallFrame;
  class Thread;
  class ImmixGC;
  class GCData;

  class ImmixMarker : public AuxiliaryThread, public Lockable {
    SharedState& shared_;
    VM* vm_;
    ImmixGC* immix_;
    GCData* data_;

    bool exit_;
    bool running_;

    TypedRoot<Thread*> thread_;

    utilities::thread::Condition run_cond_;
    utilities::thread::Mutex run_lock_;

  public:

    ImmixMarker(STATE, ImmixGC* immix);
    virtual ~ImmixMarker();

    void perform(STATE);

    void initialize(STATE);

    void start_thread(STATE);
    void stop_thread(STATE);

    void shutdown(STATE);
    void after_fork_child(STATE);

    void concurrent_mark(GCData* data);

    void run(STATE);
  };
}

#endif

