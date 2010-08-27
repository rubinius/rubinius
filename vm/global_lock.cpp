#include "global_lock.hpp"
#include "prelude.hpp"
#include "vm.hpp"

#include "builtin/nativemethod.hpp"

#include <sched.h>
#include <time.h>
#include <sys/time.h>

// This implements the GIL, which a thread must hold in order to execute.
//
// The previous implementation was just a single mutex which would be locked
// and unlocked. Because of the characteristics of the threads (namely that
// a thread that is CPU bound will yield control to another thread) that
// strategy was not enough. It was easy to starve threads from getting any
// timeslices.
//
// The new strategy is the same strategy introduced in Python 3.2 (newgil),
// namely that in uses a pair of mutex+condvars to request and signal GIL control.
// The other mutex+condvar pair is used as a handshake protocol to force
// the OS to yield control of a CPU bound thread to another thread.
//
// References:
//   http://mail.python.org/pipermail/python-dev/2009-October/093321.html
//   http://dabeaz.blogspot.com/2010/01/presentation-on-new-python-gil.html
//   http://bugs.python.org/issue7946
//
// See below for more details on the protocol.

namespace rubinius {
  GlobalLock::GlobalLock()
    : locked_(0)
    , serial_(0)
    , request_drop_(0)
  {
    timeout_.tv_sec = 0;
    timeout_.tv_usec = 10000;
  }

  void GlobalLock::init() {
    locked_ = 0;
    serial_ = 0;
    request_drop_ = 0;

    mutex_.init();
    condition_.init();

    handshake_mutex_.init();
    handshake_condition_.init();
  }

  void GlobalLock::take() {
    thread::Mutex::LockGuard lg(mutex_);

    while(locked_) {
      uint32_t saved_serial = serial_;

      struct timespec ts;
      struct timeval tv;

      gettimeofday(&tv, NULL);
      timeradd(&tv, &timeout_, &tv);
      TIMEVAL_TO_TIMESPEC(&tv, &ts);

      // Wait for the requested amount of time...
      if(condition_.wait_until(mutex_, &ts) == thread::cTimedOut) {
        // If we don't have control, ask for it.
        // We check saved_serial to serial_ to see if anyone else has woken
        // up for a requested timeslice. If they have, we don't want to interrupt
        // them, they might have started their timeslice 2 microseconds ago. So
        // instead we loop back and wait for the full timeout again before checking
        // (and hopefully asking) for the gil.
        if(locked_ && saved_serial == serial_) {
          request_drop_ = 1;
        }
      }
    }

    // Ok, locked_ isn't set, we can grab it now.

    // Handshake control back to the yielder.
    {
      thread::Mutex::LockGuard hlg(handshake_mutex_);
      locked_ = 1;
      serial_++;
      request_drop_ = 0;
      handshake_condition_.signal();
    }
  }

  void GlobalLock::drop() {
    assert(locked_ == 1);

    {
      thread::Mutex::LockGuard lg(mutex_);
      locked_ = 0;
      condition_.signal();
    }

    // Handshake with the yieldee, so that we don't starve the system.
    thread::Mutex::LockGuard hlg(handshake_mutex_);

    // request_drop_ is reset 0 here and inside the handshake of take().
    // This is expected, because the point of handshake is to force the taker
    // to run. So if the above .signal() causes control to return to a thread
    // in take() before we get to this point, then it's fine for them to say
    // "hey, don't worry about the handshake protocol, I'm back here already" by
    // setting request_drop_ to 0.
    //
    // It should be noted that this is the minority case though. The majority case
    // is what the handshake protocol is designed to fix. Namely, when a thread
    // decides to yield the GIL (yielding meaning it's CPU bound, but voluntarily
    // allowing another thread to run) that without the handshake, the unlock and
    // then relocking again right away can starve the other threads in the system.
    // This starves the system because from the OSes perspective, it's totally fine
    // to not bother to schedule another thread right away when a mutex is unlocked.
    // But this means that when that same thread that unlock goes to lock it again,
    // no other thread was able to jump in and grab the lock and do some work.
    //
    // So the handshake forces the OS to actually yield control to another thread by
    // having a thread that is dropping the GIL wait for a thread that is taking
    // the GIL to acknowledge that it has taken the GIL.
    //
    if(request_drop_ == 1) {
      request_drop_ = 0;
      handshake_condition_.wait(handshake_mutex_);
    }
  }

  void GlobalLock::yield(STATE, CallFrame* cf) {
    state->set_call_frame(cf);
    drop();

    // We handshaked with the yieldee, so it's safe to retake right away
    // without starving the system.

    take();
  }

  GlobalLock::LockGuard::LockGuard(GlobalLock& in_lock)
    : lock_(in_lock)
  {
    if(debug_locking) std::cout << "[  Locking GIL ] " << pthread_self() << "\n";
    lock_.take();
    if(debug_locking) std::cout << "[   Locked GIL ] " << pthread_self() << "\n";
  }

  GlobalLock::LockGuard::~LockGuard() {
    lock_.drop();
    if(debug_locking) std::cout << "[ Unlocked GIL ] " << pthread_self() << "\n";
  }

  GlobalLock::UnlockGuard::UnlockGuard(VM* state, CallFrame* call_frame)
    : vm_(state)
    , lock_(state->global_lock())
  {
    state->set_call_frame(call_frame);

    lock_.drop();

    if(debug_locking) std::cout << "[ Unlocked GIL ] " << pthread_self() << "\n";
  }

  GlobalLock::UnlockGuard::UnlockGuard(NativeMethodEnvironment* nme)
    : vm_(nme->state())
    , lock_(vm_->global_lock())
  {
    vm_->set_call_frame(nme->current_call_frame());

    lock_.drop();

    if(debug_locking) std::cout << "[ Unlocked GIL ] " << pthread_self() << "\n";
  }

  GlobalLock::UnlockGuard::~UnlockGuard() {
    if(debug_locking) std::cout << "[  Locking GIL ] " << pthread_self() << "\n";

    lock_.take();

    if(debug_locking) std::cout << "[   Locked GIL ] " << pthread_self() << "\n";
  }
}
