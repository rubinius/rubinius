#ifndef RBX_GC_IMMIX_MARKER_HPP
#define RBX_GC_IMMIX_MARKER_HPP

#include "lock.hpp"
#include "internal_threads.hpp"

#include "gc/root.hpp"

namespace rubinius {
  class VM;
  class State;
  struct CallFrame;
  class ImmixGC;
  class GCData;

  class ImmixMarker : public InternalThread, public Lockable {
    ImmixGC* immix_;
    GCData* data_;

    utilities::thread::Condition run_cond_;
    utilities::thread::Mutex run_lock_;

  public:

    ImmixMarker(STATE, ImmixGC* immix);
    virtual ~ImmixMarker();

    void initialize(STATE);
    void cleanup();
    void run(STATE);
    void wakeup(STATE);
    void stop(STATE);

    void after_fork_child(STATE);

    void concurrent_mark(GCData* data);
  };
}

#endif

