#ifndef RBX_GC_MANAGED_THREAD
#define RBX_GC_MANAGED_THREAD

#include "gc/slab.hpp"
#include "gc/variable_buffer.hpp"
#include "gc/root_buffer.hpp"
#include "gc/root.hpp"
#include "lock.hpp"

#include <algorithm>
#include <vector>

namespace rubinius {
  class SharedState;
  class VM;
  class WorldState;

  typedef std::vector<ObjectHeader*> LockedObjects;

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
    std::string name_;
    VariableRootBuffers variable_root_buffers_;
    RootBuffers root_buffers_;
    LockedObjects locked_objects_;
    RunState run_state_;
    Kind kind_;

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

    Kind kind() const {
      return kind_;
    }

    VM* as_vm() {
      if(kind_ == eRuby) return reinterpret_cast<VM*>(this);
      return 0;
    }

    std::string name() const {
      return name_;
    }

    void set_name(std::string name) {
      name_ = name;
      utilities::thread::Thread::set_os_name(name.c_str());
    }

    uint32_t thread_id() const {
      return id_;
    }

    pthread_t& os_thread() {
      return os_thread_;
    }

    void set_run_state(RunState s) {
      run_state_ = s;
    }

    RunState run_state() const {
      return run_state_;
    }

  public:
    static ManagedThread* current();
    static void set_current(ManagedThread* vm, std::string name);
  };
}

#endif
