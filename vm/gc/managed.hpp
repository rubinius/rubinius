#ifndef RBX_GC_MANAGED_THREAD
#define RBX_GC_MANAGED_THREAD

#include "gc/slab.hpp"
#include "gc/variable_buffer.hpp"
#include "gc/root_buffer.hpp"
#include "gc/root.hpp"
#include "lock.hpp"

#include <list>

namespace rubinius {
  class SharedState;
  class VM;
  class WorldState;

  class ManagedThread : public Lockable {
  public:
    // WorldState sets the run_state_ directly.
    friend class WorldState;

    enum Kind {
      eRuby, eSystem
    };

    enum RunState {
      eRunning,     //< Normal state. Running accessing GC memory.
      eSuspended,   //< Waiting to be told to restart
      eIndependent, //< Off running code that doesn't access GC memory.
      eAlone        //< Suspended all other threads, alone to run.
    };

  private:
    SharedState& shared_;
    Roots roots_;
    Kind kind_;
    std::string name_;
    VariableRootBuffers variable_root_buffers_;
    RootBuffers root_buffers_;
    RunState run_state_;
    std::list<ObjectHeader*> locked_objects_;

  protected:
    gc::Slab local_slab_;
    pthread_t os_thread_;
    uint32_t id_;

  public:
    ManagedThread(uint32_t id, SharedState& ss, Kind kind);

    Roots& roots() {
      return roots_;
    }

    VariableRootBuffers& variable_root_buffers() {
      return variable_root_buffers_;
    }

    RootBuffers& root_buffers() {
      return root_buffers_;
    }

    gc::Slab& local_slab() {
      return local_slab_;
    }

    std::list<ObjectHeader*>& locked_objects() {
      return locked_objects_;
    }

    void add_locked_object(ObjectHeader* obj) {
      locked_objects_.push_back(obj);
    }

    void del_locked_object(ObjectHeader* obj) {
      locked_objects_.remove(obj);
    }

    Kind kind() {
      return kind_;
    }

    VM* as_vm() {
      if(kind_ == eRuby) return reinterpret_cast<VM*>(this);
      return 0;
    }

    std::string name() {
      return name_;
    }

    void set_name(std::string name) {
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
    static void set_current(ManagedThread* vm, std::string name);
  };
}

#endif
