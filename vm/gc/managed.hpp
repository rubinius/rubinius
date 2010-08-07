#ifndef RBX_GC_MANAGED_THREAD
#define RBX_GC_MANAGED_THREAD

#include "gc/slab.hpp"
#include "gc/variable_buffer.hpp"
#include "gc/root.hpp"
#include "lock.hpp"

namespace rubinius {
  class SharedState;
  class VM;

  class ManagedThread : public Lockable {
  public:
    enum Kind {
      eRuby, eSystem
    };

    enum RunState {
      eRunning, eSuspended
    };

  private:
    SharedState& shared_;
    Roots roots_;
    Kind kind_;
    const char* name_;
    VariableRootBuffers root_buffers_;
    RunState run_state_;

  protected:
    gc::Slab local_slab_;
    pthread_t os_thread_;
    uint32_t id_;
    CallFrame* saved_call_frame_;

  public:
    ManagedThread(uint32_t id, SharedState& ss, Kind kind)
      : shared_(ss)
      , kind_(kind)
      , name_(kind == eRuby ? "<ruby>" : "<system>")
      , os_thread_(0)
      , id_(id)
    {}

    Roots& roots() {
      return roots_;
    }

    VariableRootBuffers& root_buffers() {
      return root_buffers_;
    }

    gc::Slab& local_slab() {
      return local_slab_;
    }

    Kind kind() {
      return kind_;
    }

    VM* as_vm() {
      if(kind_ == eRuby) return reinterpret_cast<VM*>(this);
      return 0;
    }

    const char* name() {
      return name_;
    }

    void set_name(const char* name) {
      name_ = name;
    }

    uint32_t thread_id() {
      return id_;
    }

    pthread_t& os_thread() {
      return os_thread_;
    }

    void set_run_state(RunState s) {
      run_state_ = s;
    }

    RunState run_state() {
      return run_state_;
    }

  public:
    static ManagedThread* current();
    static void set_current(ManagedThread* vm);
  };
}

#endif
