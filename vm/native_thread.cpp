#include "native_thread.hpp"

#include "call_frame.hpp"
#include "message.hpp"

#include "builtin/thread.hpp"
#include "builtin/string.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"
#include "object_utils.hpp"

namespace rubinius {
  NativeThread::NativeThread(VM* vm)
    : vm_(vm)
    , lock_(vm->global_lock())
    , block_(vm, Qnil)
    , args_(vm, (Array*)Qnil)
  {}

  void NativeThread::perform() {
    // Block all signals by default.
    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    // Grab the GIL
    // (automatically unlocked at the end of this function)
    GlobalLock::LockGuard x(lock_);

    CallFrame cf;
    cf.previous = NULL;
    cf.name = NULL;
    cf.cm = NULL;
    cf.top_scope = NULL;
    cf.scope = NULL;
    cf.stack_size = 0;
    cf.current_unwind = 0;
    cf.ip = 0;

    Object* ret = vm_->thread.get()->send(vm_, &cf, vm_->symbol("initialize"),
                                          args_.get(), block_.get());
    if(!ret) {
      if(Exception* exc = try_as<Exception>(vm_->thread_state()->raise_value())) {
        std::cout << "Exception at thread toplevel:\n";
        std::cout << exc->message()->c_str() << " ("
                  << exc->class_object(vm_)->name()->c_str(vm_) << ")\n\n";
        exc->print_locations(vm_);
      }
    }

  }
}
