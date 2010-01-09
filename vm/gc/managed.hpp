#ifndef RBX_GC_MANAGED_THREAD
#define RBX_GC_MANAGED_THREAD

namespace rubinius {
  class SharedState;

  class ManagedThread {
    SharedState& shared_;
    Roots roots_;

  public:
    ManagedThread(SharedState& ss)
      : shared_(ss)
    {}

    Roots& roots() {
      return roots_;
    }
  };
}

#endif
