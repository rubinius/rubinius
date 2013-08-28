#ifndef RBX_UTIL_TIMING
#define RBX_UTIL_TIMING

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

int thread_cpu_usage(uint64_t* user, uint64_t* system);

#if defined(__cplusplus)
}
#endif

#endif
