#include "shotgun.h"
#include <stdlib.h>
#include <string.h>
#include "baker.h"

baker_gc baker_gc_new(int size) {
  baker_gc g;
  g = (baker_gc)malloc(sizeof(struct baker_gc_struct));
  memset(g, 0, sizeof(struct baker_gc_struct));
  g->remember_set = g_ptr_array_new();
  
  g->space_a = heap_new(size);
  g->space_b = heap_new(size);
  g->current = g->space_a;
  g->next =    g->space_b;
  g->used =    0;
  return g;
}

int baker_gc_enlarge_next(baker_gc g, int sz) {
  rheap h;
  h = heap_new(sz);
  return baker_gc_set_next(g, h);
}

int baker_gc_set_next(baker_gc g, rheap h) {
  if(g->next == g->space_a) {
    g->space_a = h;
  } else {
    g->space_b = h;
  }
  g->next = h;
  return TRUE;
}

int baker_gc_start_address(baker_gc g) {
  return g->current->address;
}

int baker_gc_used(baker_gc g) {
  return g->used;
}

int baker_gc_swap(baker_gc g) {
  rheap tmp;
  tmp = g->current;
  g->current = g->next;
  g->next = tmp;
  
  heap_reset(tmp);
  /* Reset used to the what the current has used. */
  g->used = g->current->current - g->current->address;
  return TRUE;
}

int baker_gc_destroy(baker_gc g) {
  heap_deallocate(g->space_a);
  heap_deallocate(g->space_b);
  return TRUE;
}

address baker_gc_allocate(baker_gc g, int size) {
  address out;
  out = (address)heap_allocate(g->current, size);
  g->used += size;
  return out;
}

int baker_gc_allocate_spilled(baker_gc g, int size) {
  return heap_allocate(g->next, size);
}

#define FORWARDING_MAGIC 0xff

void baker_gc_set_forwarding_address(OBJECT obj, OBJECT dest) {
  HEADER(obj)->flags = FORWARDING_MAGIC;
  HEADER(obj)->klass = dest;
}

int baker_gc_forwarded_p(OBJECT obj) {
  return HEADER(obj)->flags == FORWARDING_MAGIC;
}

int baker_gc_forwarded_object(OBJECT obj) {
  return HEADER(obj)->klass;
}

int baker_gc_mutate_object(baker_gc g, OBJECT obj) {
  OBJECT dest;
  dest = heap_copy_object(g->next, obj);
  baker_gc_set_forwarding_address(obj, dest);
  return dest;
}

int baker_gc_contains_p(baker_gc g, OBJECT obj) {
  return heap_contains_p(g->current, obj);
}

static inline OBJECT baker_gc_maybe_mutate(baker_gc g, OBJECT iobj) {
  if(baker_gc_forwarded_p(iobj)) {
    iobj = baker_gc_forwarded_object(iobj);
  } else if(baker_gc_contains_p(g, iobj)) {
    iobj = baker_gc_mutate_object(g, iobj);
  }
  
  return iobj;
}

int _object_stores_bytes(OBJECT self);

OBJECT baker_gc_mutate_from(baker_gc g, OBJECT iobj) {
  OBJECT ret, cls, tmp;
  int i;
  
  iobj = baker_gc_maybe_mutate(g, iobj);
  
  ret = iobj;
  
  while(iobj) {
    cls = CLASS_OBJECT(iobj);
    if(REFERENCE_P(cls)) {
      cls = baker_gc_maybe_mutate(g, cls);
    }
    
    HEADER(iobj)->klass = cls;
    
    if(!_object_stores_bytes(iobj)) {
      for(i = 0; i < NUM_FIELDS(iobj); i++) {
        tmp = NTH_FIELD(iobj, i);
        // printf("   %d: %p (%d)\n", i, tmp, REFERENCE_P(tmp));
        if(!REFERENCE_P(tmp)) continue;
      
        tmp = baker_gc_maybe_mutate(g, tmp);
        SET_FIELD(iobj, i, tmp);
      }
    }
    
    iobj = heap_next_unscanned(g->next);
  }
  
  return ret;
}


#define g_ptr_array_set_index(ary, idx, val) (ary->pdata[idx] = (void*)val)

int baker_gc_collect(baker_gc g, GPtrArray *roots) {
  int i, sz, enlarged_size, c;
  OBJECT tmp, dest, root;
  rheap new_heap, old_next;
  
  enlarged_size = 0;
  old_next = NULL;
  if(heap_using_extended_p(g->current)) {
    enlarged_size = g->current->size * 2;
    new_heap = heap_new(enlarged_size);
    DEBUG("Detected heap is using extended space, enlarging to %d.\n", enlarged_size);
    if(!heap_fully_scanned_p(g->next)) {
      DEBUG("Heap to enlarged was spilled to, transfering spill first.\n");
      c = 0;
      while((tmp = heap_next_unscanned(g->next))) {
        dest = heap_copy_object(new_heap, tmp);
        baker_gc_set_forwarding_address(tmp, dest);
        c++;
      }
      DEBUG("Transfered %d objects from spill.\n", c);
    }
    old_next = g->next;
    baker_gc_set_next(g, new_heap);
  }
  
  sz = g->remember_set->len;
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(g_ptr_array_index(g->remember_set, i));
    // printf("Start at RS: %p\n", root);
    if(!REFERENCE_P(root)) { continue; }
    tmp = baker_gc_mutate_from(g, root);
    g_ptr_array_set_index(g->remember_set, i, tmp);
  }
  
  sz = roots->len;
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(g_ptr_array_index(roots, i));
    // printf("%p => %p\n", g->current->address, g->next->address);
    // printf("Start at RS (%d): %p\n", i, root);
    if(!REFERENCE_P(root)) { continue; }
    tmp = baker_gc_mutate_from(g, root);
    g_ptr_array_set_index(roots, i, tmp);
  }
  
  baker_gc_swap(g);
  /* If we enlarged next before we did the compaction, then also
     enlarge next after we swap so both areas are the same size. */
  if(enlarged_size) {
    DEBUG("Finished enlarging.\n");
    baker_gc_enlarge_next(g, enlarged_size);
  }
  if(old_next) {
    DEBUG("Cleaning up old next..\n");
    heap_deallocate(old_next);
    free(old_next);
  }
  return TRUE;
}
