#ifndef RBXTI_ATOMIC_HPP
#define RBXTI_ATOMIC_HPP

#include <stdint.h>
#include <time.h>

#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))

// On "4.1.2 (Gentoo 4.1.2 p1.1)" 32-bit, gcc sync intrinsics are missing
#if (__GNUC__ == 4 && __GNUC_MINOR__ == 1 && defined(i386))
#define X86_SYNC 1
#define X86_32_SYNC 1
#else
#define GCC_SYNC 1
#endif

#elif defined(__APPLE__)
#define APPLE_SYNC 1

#elif defined(_LP64) || defined(__LP64__) || defined(__x86_64__) || defined(__amd64__)
#define X86_SYNC 1
#define X86_64_SYNC 1

#elif defined(i386) || defined(__i386) || defined(__i386__)
#define X86_SYNC 1
#define X86_32_SYNC 1

#endif

// __sync_synchronize() was very broken until 4.4, so ignore it until then
#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 4))
#define GCC_BARRIER 1

#elif defined(_LP64) || defined(__LP64__) || defined(__x86_64__) || defined(__amd64__)
#define X86_BARRIER 1

#elif defined(i386) || defined(__i386) || defined(__i386__)
#define X86_BARRIER 1

// Last so we prefer the raw instructions on x86
#elif defined(__APPLE__)
#define APPLE_BARRIER 1

#endif

#if defined(_LP64) || defined(__LP64__) || defined(__x86_64__) || defined(__amd64__)
#define X86_PAUSE 1
#elif defined(i386) || defined(__i386) || defined(__i386__)
#define X86_PAUSE 1
#endif

#if defined(APPLE_SYNC) || defined(APPLE_BARRIER)
#include <libkern/OSAtomic.h>
#endif

namespace rbxti {
namespace atomic {

  typedef volatile int atomic_int_t;

  inline void memory_barrier() {
#if defined(GCC_BARRIER)
    __sync_synchronize();
#elif defined(APPLE_BARRIER)
    OSMemoryBarrier();
#elif defined(X86_BARRIER)
    __asm__ __volatile__ ("mfence" ::: "memory");
#else
#error "no memory barrier implementation"
#endif
  }

  inline void pause() {
#if defined(X86_PAUSE)
    __asm__ __volatile__ ("rep; nop" ::: "memory");
#else
    struct timespec ts = {0, 0};
    nanosleep(&ts, NULL);
#endif
  }

  inline bool compare_and_swap(uint32_t* ptr, uint32_t old_val, uint32_t new_val) {
#if defined(GCC_SYNC)
    return __sync_bool_compare_and_swap(ptr, old_val, new_val);
#elif defined(APPLE_SYNC)
    return OSAtomicCompareAndSwap32Barrier(old_val, new_val, (volatile int32_t*)ptr);
#elif defined(X86_SYNC)
    char result = 0;

    __asm__ __volatile__ (
        "lock; cmpxchgl %4, %1; sete %0"
      : "=q" (result), "=m" (*ptr)
      : "m" (*ptr), "a" (old_val), "r" (new_val)
      : "memory");

    return result;
#else
#error "no sync primitive found"
#endif
  }

  // Because C++ gets twitchy (ie, errors out) if it has to use uint32_t for
  // any signed integer types.
  inline bool compare_and_swap(int32_t* ptr, int32_t old_val, int32_t new_val) {
    return compare_and_swap(
        reinterpret_cast<uint32_t*>(ptr), (uint32_t)old_val, (uint32_t)new_val);
  }

  inline bool compare_and_swap(uint64_t* ptr, uint64_t old_val, uint64_t new_val) {
#if defined(GCC_SYNC)
    return __sync_bool_compare_and_swap(ptr, old_val, new_val);
#elif defined(APPLE_SYNC)
    return OSAtomicCompareAndSwap64Barrier(old_val, new_val, (volatile int64_t*)ptr);
#elif defined(X86_32_SYNC)
    char result = 0;

    int old_val_hi = old_val >> 32;
    int old_val_lo = old_val & 0xffffffff;

    int new_val_hi = new_val >> 32;
    int new_val_lo = new_val & 0xffffffff;

    __asm__ __volatile__ (
      "push %%ebx; mov %5, %%ebx;"
      "lock; cmpxchg8b %1; sete %0;"
      "pop %%ebx"
    : "=q" (result)
    : "m" (*ptr), "d" (old_val_hi), "a" (old_val_lo),
                  "c" (new_val_hi), "r" (new_val_lo)
    : "memory");

    return result;
#elif defined(X86_64_SYNC)
    char result = 0;

    __asm__ __volatile__ (
        "lock; cmpxchgq %4, %1; sete %0"
      : "=q" (result), "=m" (*ptr)
      : "m" (*ptr), "a" (old_val), "r" (new_val)
      : "memory");

    return result;
#else
#error "no sync primitive found"
#endif
  }

  inline bool compare_and_swap(void** ptr, void* old_val, void* new_val) {
#if defined(GCC_SYNC)
    return __sync_bool_compare_and_swap(ptr, old_val, new_val);
#elif defined(APPLE_SYNC)
    return OSAtomicCompareAndSwapPtrBarrier(old_val, new_val, ptr);
#elif defined(X86_64_SYNC)
    return compare_and_swap(
      reinterpret_cast<uint64_t*>(ptr),
      reinterpret_cast<uint64_t>(old_val),
      reinterpret_cast<uint64_t>(new_val));
#elif defined(X86_32_SYNC)
    return compare_and_swap(
      reinterpret_cast<uint32_t*>(ptr),
      reinterpret_cast<uint32_t>(old_val),
      reinterpret_cast<uint32_t>(new_val));
#else
#error "no sync prims defined"
#endif
  }

  template <typename intish>
  inline intish fetch_and_add(intish* ptr, intish inc) {
#if defined(GCC_SYNC)
    return __sync_fetch_and_add(ptr, inc);
#else
    intish val = *ptr;
    while(!compare_and_swap(ptr, val, val + inc)) {
      val = *ptr;
    }
    return val;
#endif
  }

  template <typename intish>
  inline intish fetch_and_sub(intish* ptr, intish inc) {
#if defined(GCC_SYNC)
    return __sync_fetch_and_sub(ptr, inc);
#else
    intish val = *ptr;
    while(!compare_and_swap(ptr, val, val - inc)) {
      val = *ptr;
    }
    return val;
#endif
  }

  template <typename intish>
  inline intish test_and_set(intish *ptr) {
#if defined(GCC_SYNC)
    return __sync_lock_test_and_set(ptr, 1);
#elif defined(APPLE_SYNC)
    return OSAtomicTestAndSetBarrier(0, (volatile void*)ptr);
#elif defined(X86_SYNC)
    return !compare_and_swap((uint32_t*)ptr, 0, 1);
#else
#error "no sync primitive found"
#endif
  }

  template <typename intish>
  inline void test_and_clear(intish *ptr) {
#if defined(GCC_SYNC)
    __sync_lock_release(ptr);
#elif defined(APPLE_SYNC)
    OSAtomicTestAndClearBarrier(0, (volatile void*)ptr);
#elif defined(X86_SYNC)
    memory_barrier();
    *ptr = 0;
#else
#error "no sync primitive found"
#endif
  }

}

class SpinLock {
private:
  int lock_;

public:
  SpinLock()
    : lock_(0)
  {}

  void init() {
    lock_ = 0;
  }

  void lock() {
    while(!atomic::compare_and_swap(&lock_, 0, 1)) {
      while(lock_) {
        atomic::pause();
      }
    }
  }

  void unlock() {
    atomic::compare_and_swap(&lock_, 1, 0);
  }

  bool try_lock() {
    if(atomic::compare_and_swap(&lock_, 0, 1)) {
      return true;
    }

    return false;
  }
};
}


#endif
