#ifndef __RUBINIUS_HEAP__
#define __RUBINIUS_HEAP__ 1

typedef uintptr_t address;

struct heap {
  int size;
  address address;
  address current;
  address last;
  address scan;
};

typedef struct heap* rheap;

rheap heap_new(int size);
int heap_deallocate(rheap h);
int heap_allocate_memory(rheap h);
int heap_allocate_extended(rheap h);
int heap_reset(rheap h);
int heap_contains_p(rheap h, address addr);
int heap_enough_fields_p(rheap h, int fields);
int heap_allocated_p(rheap h);
int heap_using_extended_p(rheap h);
OBJECT heap_copy_object(rheap h, OBJECT obj);
OBJECT heap_next_object(rheap h);
OBJECT heap_fully_scanned_p(rheap h);
OBJECT heap_next_unscanned(rheap h);
int heap_enough_fields_p(rheap h, int fields);

#define FAST_HEAP 1

#ifdef FAST_HEAP

#include <string.h>

static inline address heap_allocate(rheap h, int size) {
  address addr;
  addr = (address)h->current;
  memset((void*)addr, 0, size);
  h->current += size;
  
  return addr;
}

#define heap_allocate_dirty(h, size) ({ \
  address _a; _a = (address)h->current; h->current += size; _a; })

static inline int heap_enough_space_p(rheap h, int size) {
  if(h->current + size > h->last) return FALSE;
  return TRUE;
}

// #define heap_enough_space_p(h, size) ((h)->current + size <= (h)->last)

// #define heap_allocate(h, size) ({ address addr = (address)((h)->current); (h)->current += size; addr; })

#else

address heap_allocate(rheap h, int size);
int heap_enough_space_p(rheap h, int size);

#endif

#endif

