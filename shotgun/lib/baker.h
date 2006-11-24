#include "heap.h"
#include <glib.h>

#ifndef __RUBINIUS_BAKER__
#define __RUBINIUS_BAKER__ 1

struct baker_gc_struct {
  rheap space_a;
  rheap space_b;
  rheap current;
  rheap next;
  int used;
  GPtrArray *remember_set;
};

typedef struct baker_gc_struct* baker_gc;

baker_gc baker_gc_new(int size);
int baker_gc_start_address(baker_gc g);
int baker_gc_used(baker_gc g);
int baker_gc_swap(baker_gc g);
int baker_gc_destroy(baker_gc g);
address baker_gc_allocate(baker_gc g, int size);
int baker_gc_set_next(baker_gc g, rheap h);
int baker_gc_allocate_spilled(baker_gc g, int size);
void baker_gc_set_forwarding_address(OBJECT obj, OBJECT dest);
int baker_gc_forwarded_p(OBJECT obj);
int baker_gc_forwarded_object(OBJECT obj);
int baker_gc_mutate_object(baker_gc g, OBJECT obj);
int baker_gc_contains_p(baker_gc g, OBJECT obj);
OBJECT baker_gc_mutate_from(baker_gc g, OBJECT iobj);
int baker_gc_collect(baker_gc g, GPtrArray *roots);

#endif

