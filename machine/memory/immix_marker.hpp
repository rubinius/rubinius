#ifndef RBX_GC_IMMIX_MARKER_HPP
#define RBX_GC_IMMIX_MARKER_HPP

#include "machine_threads.hpp"

#include "memory/root.hpp"

namespace rubinius {
  class VM;
  class State;
  struct CallFrame;

namespace memory {
  class ImmixGC;
  class GCData;

  class ImmixMarker : public MachineThread {
    ImmixGC* immix_;
    GCData* data_;

  public:

    ImmixMarker(STATE, ImmixGC* immix, GCData* data);
    virtual ~ImmixMarker();

    void initialize(STATE);
    void cleanup();
    void run(STATE);
    void stop(STATE);

    void after_fork_child(STATE);
  };
}
}

#endif
