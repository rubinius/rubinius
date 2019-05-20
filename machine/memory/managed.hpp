#ifndef RBX_GC_MANAGED_THREAD
#define RBX_GC_MANAGED_THREAD

#include "defines.hpp"
#include "memory/slab.hpp"
#include "memory/variable_buffer.hpp"
#include "memory/root_buffer.hpp"
#include "memory/root.hpp"

#include "diagnostics.hpp"

#include <algorithm>
#include <vector>
#include <string>

namespace rubinius {
  class SharedState;
  class VM;

namespace memory {
  class ManagedThread {
  public:
    enum Kind {
      eThread,
      eFiber,
      eSystem
    };

  private:
    Roots roots_;
    std::string name_;
    VariableRootBuffers variable_root_buffers_;
    RootBuffers root_buffers_;
    Kind kind_;
    diagnostics::MachineMetrics* metrics_;

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

    diagnostics::MachineMetrics* metrics() {
      return metrics_;
    }
  };
}
}

#endif
