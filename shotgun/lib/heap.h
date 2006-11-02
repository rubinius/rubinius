#ifndef __RUBINIUS_HEAP__
#define __RUBINIUS_HEAP__ 1

struct heap {
  int size;
  int address;
  int current;
  int last;
  int scan;
};

typedef struct heap* rheap;
typedef unsigned long address;

rheap heap_new(int size);
int heap_deallocate(rheap h);
int heap_allocate_memory(rheap h);
int heap_reset(rheap h);
int heap_contains_p(rheap h, int addr);
int heap_allocated_p(rheap h);
address heap_allocate(rheap h, int size);
int heap_enough_space_p(rheap h, int size);
OBJECT heap_copy_object(rheap h, OBJECT obj);
OBJECT heap_next_object(rheap h);
OBJECT heap_fully_scanned_p(rheap h);
OBJECT heap_next_unscanned(rheap h);

#endif

