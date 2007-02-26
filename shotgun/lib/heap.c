#include "shotgun.h"
#include "heap.h"
#include <stdlib.h>
#include <string.h>

rheap heap_new(int size) {
  rheap h = (rheap)malloc(sizeof(struct heap));
  memset((void*)h, 0, sizeof(struct heap));
  h->size = size;
  heap_allocate_memory(h);
  return h;
}

int heap_deallocate(rheap h) {
  if(!heap_allocated_p(h)) {
    return FALSE;
  }
  if(h->address) {
    free((void*)(h->address));
  }
  h->size = 0;
  h->address = 0;
  h->current = 0;
  /* Maybe free() here? */
  return TRUE;
}

int heap_allocate_memory(rheap h) {
  h->address = (address)malloc(h->size);
  if(!h->address) {
    return FALSE;
  }
  h->scan = h->address;
  memset((void*)(h->address), 0, h->size);
  h->last = h->address + h->size;
  return heap_reset(h);
}

int heap_reset(rheap h) {
  h->current = h->address;
  h->scan = h->current;
  return TRUE;
}

int heap_contains_p(rheap h, address addr) {

  if(addr < h->address) return FALSE;
  if(addr >= h->address + h->size) return FALSE;
  return TRUE;
}

int heap_allocated_p(rheap h) {
  return h->address > 0;
}

#ifndef FAST_HEAP

address heap_allocate(rheap h, int size) {
  address addr;
  /* maybe raise exception here? */
  assert(heap_enough_space_p(h, size));
  addr = (address)h->current;
  assert(addr < h->last);
  memset((void*)addr, 0, size);
  h->current += size;
  
  return addr;
}

int heap_enough_space_p(rheap h, int size) {
  if (size < 0) abort();
  if(h->current + size > h->last) return FALSE;
  return TRUE;
}

#endif

int heap_enough_fields_p(rheap h, int fields) {
  int size;
  
  size = (HEADER_SIZE + fields) * REFSIZE;
  
  if(h->current + size > h->last) return FALSE;
  return TRUE;
}

OBJECT heap_copy_object(rheap h, OBJECT obj) {
  address out;
  int size;
  if(heap_contains_p(h, obj)) return obj;
  size = SIZE_IN_BYTES(obj);
  
  out = heap_allocate(h, size);
  if(!out) return 0;
  memcpy((void*)out, (void*)obj, size);
  return (OBJECT)out;
}

/* Functions to support the cheney scan algorithm. */

OBJECT heap_next_object(rheap h) {
  return (OBJECT)(h->current);
}

OBJECT heap_fully_scanned_p(rheap h) {
  return h->scan == h->current;
}

OBJECT heap_next_unscanned(rheap h) {
  OBJECT obj;
  if(heap_fully_scanned_p(h)) return 0;
  obj = (OBJECT)(h->scan);
  h->scan += SIZE_IN_BYTES(obj);
  return obj;  
}
