
#include "maps.hpp"

namespace rubinius {
  class SharedState;
  class VM;

  class VMManager {
    int share_id_;
    int vm_id_;

    ShareMap shares_;
    VMMap vms_;
  public:

    VMManager()
      : share_id_(0)
      , vm_id_(0)
    {}

    SharedState* create_shared_state();
    VM* create_vm(SharedState*);

    void destroy_vm(VM*);
    void prune();
  };
}
