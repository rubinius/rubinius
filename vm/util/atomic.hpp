#ifndef RBX_UTIL_ATOMIC_HPP
#define RBX_UTIL_ATOMIC_HPP

#include <stdint.h>

#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))
#define GCC_SYNC 1

#elif defined(__APPLE__)
#define APPLE_SYNC 1

#elif defined(_LP64) || defined(__LP64__) || defined(__x86_64__) || defined(__amd64__)
#define X86_SYNC 1
#define X86_64_SYNC 1

#elif defined(i386) || defined(__i386) || defined(__i386__)
#define X86_SYNC 1
#define X86_32_SYNC 1

#endif

namespace atomic {

  inline void memory_barrier() {
#if defined(GCC_SYNC)
    __sync_synchronize();
#elif defined(APPLE_SYNC)
    OSMemoryBarrier();
#elif defined(X86_SYNC)
    __asm__ __volatile__ ("mfence" ::: "memory");
#endif
  }

  inline bool compare_and_swap(uint32_t* ptr, uint32_t old_val, uint32_t new_val) {
#if defined(GCC_SYNC)
    return __sync_bool_compare_and_swap(ptr, old_val, new_val);
#elif defined(APPLE_SYNC)
    return OSAtomicCompareAndSwap32Barrier(old_val, new_val, ptr);
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
    return OSAtomicCompareAndSwap64Barrier(old_val, new_val, ptr);
#elif defined(X86_32_SYNC)
    char result = 0;

    int old_val_hi = old_val >> 32;
    int old_val_lo = old_val & 0xffffffff;

    int new_val_hi = new_val >> 32;
    int new_val_lo = new_val & 0xffffffff;

    __asm__ __volatile__ (
        "lock; cmpxchg8b %1; sete %0"
      : "=q" (result)
      : "m" (*ptr), "d" (old_val_hi), "a" (old_val_lo),
                    "c" (new_val_hi), "b" (new_val_hi)
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
}

#endif
