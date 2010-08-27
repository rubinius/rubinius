#include "native_thread.hpp"

#include "call_frame.hpp"
#include "arguments.hpp"
#include "exception.hpp"

#include "builtin/thread.hpp"
#include "builtin/string.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"
#include "object_utils.hpp"
#include "builtin/nativemethod.hpp"

namespace rubinius {
  NativeThread::NativeThread(VM* vm, size_t stack_size, pthread_t tid)
    : thread::Thread(stack_size, tid)
    , vm_(vm)
  {}

  void NativeThread::perform() {
    // Grab the GIL
    // (automatically unlocked at the end of this function)
    GlobalLock::LockGuard x(vm_->global_lock());

    NativeMethod::init_thread(vm_);
    VM::set_current(vm_);

    // Register that when the perform returns and the thread is exitting, to
    // run delete on this object to free up the memory.
    set_delete_on_exit();

    int stack_boundary = 0;
    vm_->set_stack_bounds(reinterpret_cast<uintptr_t>(&stack_boundary), stack_size());

    Object* ret = vm_->thread.get()->send(vm_, NULL, vm_->symbol("__run__"));

    if(!ret) {
      if(vm_->thread_state()->raise_reason() == cExit) {
        if(Fixnum* fix = try_as<Fixnum>(vm_->thread_state()->raise_value())) {
          exit(fix->to_native());
        } else {
          exit(-1);
        }
      }

      if(Exception* exc = try_as<Exception>(vm_->thread_state()->raise_value())) {
        std::cout << "Exception at thread toplevel:\n";

        std::cout << exc->message_c_str() << " (";

        std::cout << exc->class_object(vm_)->name()->c_str(vm_) << ")\n\n";
        // This can blow up. Don't do it.
        // exc->print_locations(vm_);
      }
    }

    NativeMethod::cleanup_thread(vm_);

    vm_->thread.get()->detach_native_thread();
    VM::discard(vm_);
    vm_ = NULL;
  }
}
