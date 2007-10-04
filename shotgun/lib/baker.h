#include "heap.h"
#include <glib.h>
#include <ptr_array.h>
#ifndef __RUBINIUS_BAKER__
#define __RUBINIUS_BAKER__ 1

struct baker_gc_struct {
  rheap space_a;
  rheap space_b;
  rheap current;
  rheap next;
  int used;
  int tenure_age;
  ptr_array remember_set;
  void *tenure_data;
  OBJECT (*tenure)(void*, OBJECT obj);
  int tenure_now;
  void *om;
  ptr_array seen_weak_refs;
  OBJECT become_from, become_to;
  char *last_start, *last_end;
  int num_collection;
};

typedef struct baker_gc_struct* baker_gc;

baker_gc baker_gc_new(int size);
address baker_gc_start_address(baker_gc g);
int baker_gc_used(baker_gc g);
int baker_gc_swap(baker_gc g);
int baker_gc_destroy(baker_gc g);
address baker_gc_allocate(baker_gc g, int size);
int baker_gc_set_next(baker_gc g, rheap h);
address baker_gc_allocate_spilled(baker_gc g, int size);
void baker_gc_set_forwarding_address(OBJECT obj, OBJECT dest);
OBJECT baker_gc_forwarded_object(OBJECT obj);
OBJECT baker_gc_mutate_object(STATE, baker_gc g, OBJECT obj);
int baker_gc_contains_p(baker_gc g, OBJECT obj);
int baker_gc_contains_spill_p(baker_gc g, OBJECT obj);
OBJECT baker_gc_mutate_from(STATE, baker_gc g, OBJECT iobj);
int baker_gc_collect(STATE, baker_gc g, ptr_array roots);
void baker_gc_clear_gc_flag(baker_gc g, int flag);
void baker_gc_describe(baker_gc g);
void baker_gc_find_lost_souls(STATE, baker_gc g);
void baker_gc_collect_references(STATE, baker_gc g, OBJECT mark, ptr_array refs);
inline int baker_gc_forwarded_p(OBJECT obj);
void baker_gc_mutate_context(STATE, baker_gc g, OBJECT iobj, int shifted, int top);



#define baker_gc_allocate_ultra(g, size) heap_allocate_dirty((g)->current, size)
#define baker_gc_allocate_spilled_ultra(g, size) heap_allocate_dirty((g)->next, size)

// #define MEM_DEBUG 1

#ifdef MEM_DEBUG

#define baker_gc_allocate(g, size) (heap_allocate((g)->current, size))
#define baker_gc_allocate_spilled(g, size) (heap_allocate((g)->next, size))

#else

#define baker_gc_allocate(g, size) baker_gc_allocate_ultra(g, size);
#define baker_gc_allocate_spilled(g, size) baker_gc_allocate_spilled_ultra(g, size);

#endif

#endif

