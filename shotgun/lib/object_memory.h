#include "shotgun.h"
#include "baker.h"
#include "marksweep.h"
#include <glib.h>
#include <stdlib.h>

#ifndef __RUBINIUS_OM__
#define __RUBINIUS_OM__ 1

#define OMDefaultSize 1048576
/* A little over 1% of the total heap size. */
#define LargeObjectThreshold 2700
//#define OMDefaultSize 100000

#define OMCollectYoung  0x1
#define OMCollectMature 0x2

struct object_memory_struct {
  int collect_now;
  int enlarge_now;
  int tenure_now;
  int new_size;
  int last_object_id;
  baker_gc gc;
  mark_sweep_gc ms;
  int last_tenured;
};

typedef struct object_memory_struct *object_memory;

object_memory object_memory_new();
int object_memory_delete(object_memory om);
int object_memory_used(object_memory om);
int object_memory_collect(STATE, object_memory om, GPtrArray *roots);
void object_memory_check_memory(object_memory om);
OBJECT object_memory_new_object_normal(object_memory om, OBJECT cls, int fields);
static inline OBJECT _om_inline_new_object(object_memory om, OBJECT cls, int fields);

OBJECT object_memory_new_object_mature(object_memory om, OBJECT cls, int fields);
void object_memory_print_stats(object_memory om);
OBJECT object_memory_new_opaque();
OBJECT object_memory_tenure_object(void* data, OBJECT obj);
void object_memory_major_collect(STATE, object_memory om, GPtrArray *roots);
OBJECT object_memory_collect_references(STATE, object_memory om, OBJECT mark);
void object_memory_setup_become(STATE, object_memory om, OBJECT from, OBJECT to);
void object_memory_clear_become(STATE, object_memory om);

#define FAST_NEW 1

#ifdef FAST_NEW
#define object_memory_new_object _om_inline_new_object
#else
#define object_memory_new_object object_memory_new_object_normal
#endif

#endif

