#ifndef RBX_NATIVE_THREAD_HPP
#define RBX_NATIVE_THREAD_HPP

#include "vm.hpp"

#include "builtin/array.hpp"

namespace rubinius {
  class NativeThread : public thread::Thread {
    VM* vm_;

  public:
    NativeThread(VM*, size_t stack_size = 0, pthread_t tid = 0);
    void perform();

    VM* vm() {
      return vm_;
    }

    static void block_all_signals() {
      sigset_t set;
      sigfillset(&set);

      pthread_sigmask(SIG_SETMASK, &set, NULL);
    }

    const static int cWakeupSignal = SIGVTALRM;

    int begin();
  };
}

#endif
