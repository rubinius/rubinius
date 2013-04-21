#ifndef RBX_UTIL_GC_ALLOC
#define RBX_UTIL_GC_ALLOC

#ifndef RBX_WINDOWS
#include <sys/mman.h>
#endif

namespace memory {

#ifndef RBX_WINDOWS
  static inline void* gc_alloc(std::size_t size) {
    void* addr = mmap(0, size, PROT_READ | PROT_WRITE,
                        MAP_ANON | MAP_PRIVATE, -1, 0);
    if(addr == MAP_FAILED) {
      perror("mmap");
      ::abort();
    }
    return addr;
  }

  static inline void gc_free(void* addr, std::size_t size) {
    int ret = munmap(addr, size);
    if(ret != 0) {
      perror("munmap");
      ::abort();
    }
  }
#else
  static inline void* gc_alloc(std::size_t size) {
    LPVOID addr = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE,
          PAGE_READWRITE);
    if(addr == NULL) {
      perror("VirtualAlloc");
      ::abort();
    }
    return addr;
  }

  static inline void gc_free(void* addr, std::size_t size) {
    BOOL ret = VirtualFree(addr, 0, MEM_RELEASE);
    if(!ret) {
      perror("VirtualFree");
      ::abort();
    }
  }
#endif
}
#endif
