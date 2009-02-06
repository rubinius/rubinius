#include "vm_manager.hpp"
#include "vm.hpp"

namespace rubinius {
  SharedState* VMManager::create_shared_state() {
    SharedState* shared = new SharedState(share_id_++);
    shares_[shared->id()] = shared;
    return shared;
  }

  VM* VMManager::create_vm(SharedState* shared) {
    VM* vm = new VM(*shared, vm_id_++);
    vms_[vm->id()] = vm;

    shared->ref();
    return vm;
  }

  void VMManager::destroy_vm(VM* vm) {
    SharedState* shared = &vm->shared;
    VMMap::iterator i = vms_.find(vm->id());
    assert(i != vms_.end());

    vms_.erase(i);
    delete vm;
    if(shared->deref()) {
      ShareMap::iterator si = shares_.find(shared->id());
      assert(si != shares_.end());
      shares_.erase(si);
      delete shared;
    }
  }
}
