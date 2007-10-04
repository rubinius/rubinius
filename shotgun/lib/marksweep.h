#ifndef __MARKSWEEP_H__
#define __MARKSWEEP_H__

#include <time.h>

struct ms_header;

struct ms_entry {
  int bytes;
  int fields;
  int marked;
  struct ms_header *object;
};

struct ms_header {
  struct ms_entry *entry;
};

struct ms_chunk;

struct ms_chunk {
  int size;
  int num_entries;
  int next_entry;
  struct ms_entry *entries;
  struct ms_chunk *next;
};

typedef struct ms_chunk ms_chunk;

struct _mark_sweep_gc {
  struct ms_chunk *chunks;
  void *extreme_min;
  void *extreme_max;
  ptr_array remember_set;
  int enlarged;
  int num_chunks;
  OBJECT become_from, become_to;
  ptr_array seen_weak_refs;
  ms_chunk *current;
  
  int last_freed;
  int last_marked;
  unsigned int allocated_bytes;
  int next_collection_objects;
  unsigned int last_allocated;
  unsigned int allocated_objects;
  int next_collection_bytes;
  
  clock_t last_clock;
  OBJECT track;
  
  struct ms_entry *free_list;
};

typedef struct _mark_sweep_gc *mark_sweep_gc;

#define MS_MARK 0x80
#define MS_CHUNKSIZE 0x20000
#define MS_COLLECTION_FREQUENCY 500 // 500 fields

mark_sweep_gc mark_sweep_new();
void mark_sweep_adjust_extremes(mark_sweep_gc ms, ms_chunk *new);
void mark_sweep_add_chunk(mark_sweep_gc ms);
void mark_sweep_free_chunk(mark_sweep_gc ms, ms_chunk *chunk);
OBJECT mark_sweep_allocate(mark_sweep_gc ms, int obj_fields);
void mark_sweep_free(mark_sweep_gc ms, OBJECT obj);
void mark_sweep_free_fast(STATE, mark_sweep_gc ms, OBJECT obj);
int mark_sweep_contains_p(mark_sweep_gc ms, OBJECT obj);
OBJECT mark_sweep_mark_object(STATE, mark_sweep_gc ms, OBJECT iobj);
void mark_sweep_mark_phase(STATE, mark_sweep_gc ms, ptr_array roots);
void mark_sweep_sweep_phase(STATE, mark_sweep_gc ms);
void mark_sweep_collect(STATE, mark_sweep_gc ms, ptr_array roots);
void mark_sweep_describe(mark_sweep_gc ms);
void mark_sweep_collect_references(STATE, mark_sweep_gc ms, OBJECT mark, ptr_array refs);
void mark_sweep_mark_context(STATE, mark_sweep_gc ms, OBJECT iobj);
void mark_sweep_clear_mark(STATE, OBJECT iobj);

#endif /* __MARKSWEEP_H__ */
