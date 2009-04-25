#include "vm_manager.hpp"
#include "vm.hpp"

namespace rubinius {
  SharedState* VMManager::create_shared_state() {
    SharedState* shared = new SharedState(*this, share_id_++);
    shares_[shared->id()] = shared;
    return shared;
  }

  /** @see SharedState::new_vm. */
  VM* VMManager::create_vm(SharedState* shared) {
    VM* vm = new VM(*shared, vm_id_++);
    vms_[vm->id()] = vm;

    shared->ref();
    return vm;
  }

  /**
   *  @todo This and remove_vm() do not act together in the
   *        same way that create_vm() and new_vm() do. Clean
   *        it up. --rue
   *
   *  @see  SharedState::remove_vm
   */
  void VMManager::destroy_vm(VM* vm) {
    SharedState* shared = &vm->shared;
    shared->remove_vm(vm);

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

  void VMManager::prune() {
    for(VMMap::iterator i = vms_.begin();
        i != vms_.end();
        i++) {
      VM* vm = i->second;

      /* TODO: This is not a fix, but there's no reason to segfault here
       * if we can avoid it. Find out how a NULL vm gets in the list.
       */
      if(!vm) continue;

      if(!vm->alive_p()) {
        SharedState* shared = &vm->shared;
        shared->remove_vm(vm);

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
  }
}
