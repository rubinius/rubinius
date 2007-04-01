#ifndef __MARKSWEEP_H__
#define __MARKSWEEP_H__

struct ms_chunk;

struct ms_chunk {
  int size;
  void *address;
  void *last_address;
  struct ms_chunk *next;
};

typedef struct ms_chunk ms_chunk;

struct _mark_sweep_gc {
  int chunk_size;
  struct ms_chunk *chunks;
  OBJECT free_list;
  void *extreme_min;
  void *extreme_max;
  GPtrArray *remember_set;
  int enlarged;
  int num_chunks;
};

typedef struct _mark_sweep_gc *mark_sweep_gc;

#define MS_MARK 0x80
#define MS_CHUNKSIZE 0x20000

mark_sweep_gc mark_sweep_new(int chunk_size);
void mark_sweep_adjust_extremes(mark_sweep_gc ms, ms_chunk *new);
void mark_sweep_add_chunk(mark_sweep_gc ms);
void mark_sweep_free_chunk(mark_sweep_gc ms, ms_chunk *chunk);
OBJECT mark_sweep_allocate(mark_sweep_gc ms, int obj_fields);
void mark_sweep_free(mark_sweep_gc ms, OBJECT obj);
void mark_sweep_free_fast(STATE, mark_sweep_gc ms, OBJECT obj);
int mark_sweep_contains_p(mark_sweep_gc ms, OBJECT obj);
void mark_sweep_mark_object(mark_sweep_gc ms, OBJECT iobj);
void mark_sweep_mark_phase(STATE, mark_sweep_gc ms, GPtrArray *roots);
void mark_sweep_sweep_phase(STATE, mark_sweep_gc ms);
void mark_sweep_collect(STATE, mark_sweep_gc ms, GPtrArray *roots);
void mark_sweep_describe(mark_sweep_gc ms);
void mark_sweep_collect_references(STATE, mark_sweep_gc ms, OBJECT mark, GPtrArray *refs);

#endif /* __MARKSWEEP_H__ */
