#ifndef RBX_ALLOC_HPP
#define RBX_ALLOC_HPP

#include <stddef.h>

namespace rubinius {
#define FREE(obj) free(obj)
#define ALLOC_N(type, size) ((type*)calloc((size), sizeof(type)))
#define ALLOC(t) (t*)XMALLOC(sizeof(t))
#define REALLOC_N(v,t,n) (v)=(t*)realloc((void*)(v), sizeof(t)*n)

#define ALLOCA_N(type, size) ((type*)alloca(sizeof(type) * (size)))
#define ALLOCA(type) ((type*)alloca(sizeof(type)))
};

extern "C" {
  void* XMALLOC(size_t bytes);
  void  XFREE(void* ptr);
  void* XREALLOC(void* ptr, size_t bytes);
  void* XCALLOC(size_t items, size_t bytes);
}

#endif
