#ifndef RBX_GC_MANAGED_THREAD
#define RBX_GC_MANAGED_THREAD

#include "defines.hpp"
#include "memory/slab.hpp"
#include "memory/variable_buffer.hpp"
#include "memory/root_buffer.hpp"
#include "memory/root.hpp"
#include "metrics.hpp"

#include <algorithm>
#include <vector>
#include <string>

namespace rubinius {
  class SharedState;
  class VM;

namespace memory {
  typedef std::vector<ObjectHeader*> LockedObjects;

  class ManagedThread {
  public:
    enum Kind {
      eThread,
      eFiber,
      eSystem
    };

  private:
    SharedState& shared_;
    Roots roots_;
    std::string name_;
    VariableRootBuffers variable_root_buffers_;
    RootBuffers root_buffers_;
    LockedObjects locked_objects_;
    Kind kind_;
    metrics::MetricsData metrics_;

  protected:
    memory::Slab local_slab_;
    pthread_t os_thread_;
    uint32_t id_;

  public:
    ManagedThread(uint32_t id, SharedState& ss, Kind kind, const char* name);
    ~ManagedThread();

    static void set_current_thread(ManagedThread* vm);
    static ManagedThread* current();

    Roots& roots() {
      return roots_;
    }

    VariableRootBuffers& variable_root_buffers() {
      return variable_root_buffers_;
    }

    RootBuffers& root_buffers() {
      return root_buffers_;
    }

    memory::Slab& local_slab() {
      return local_slab_;
    }

    LockedObjects& locked_objects() {
      return locked_objects_;
    }

    void add_locked_object(ObjectHeader* obj) {
      locked_objects_.push_back(obj);
    }

    void del_locked_object(ObjectHeader* obj) {
      // Often we will remove the last locked object
      // because how locks are used in a stack wise manner.
      // Therefore we optimize here for this case and have a fast path
      if(locked_objects_.size() == 0) return;
      ObjectHeader* last = locked_objects_.back();
      if(obj == last) {
        locked_objects_.pop_back();
      } else {
        LockedObjects::iterator f = std::find(locked_objects_.begin(), locked_objects_.end(), obj);
        if(f != locked_objects_.end()) {
          locked_objects_.erase(f);
        }
      }
    }

    const char* kind_name() const {
      switch(kind_) {
        case eThread:
          return "Thread";
        case eFiber:
          return "Fiber";
        case eSystem:
          return "MachineThread";
      }

      /* GCC cannot determine that the above switch covers the enum and hence
       * every exit from this function is covered.
       */
      return "unknown kind";
    }

    Kind kind() const {
      return kind_;
    }

    void set_kind(Kind kind) {
      kind_ = kind;
    }

    VM* as_vm() {
      return reinterpret_cast<VM*>(this);
    }

    std::string name() const {
      return name_;
    }

    void set_name(STATE, const char* name);

    uint32_t thread_id() const {
      return id_;
    }

    pthread_t& os_thread() {
      return os_thread_;
    }

    metrics::MetricsData& metrics() {
      return metrics_;
    }
  };
}
}

#endif
