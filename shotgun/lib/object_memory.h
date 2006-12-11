#include "shotgun.h"
#include "baker.h"
#include <glib.h>
#include <stdlib.h>

#ifndef __RUBINIUS_OM__
#define __RUBINIUS_OM__ 1

#define OMDefaultSize 648000
//#define OMDefaultSize 100000

struct object_memory_struct {
  int collect_now;
  int enlarge_now;
  int new_size;
  baker_gc gc;  
};

typedef struct object_memory_struct *object_memory;

object_memory object_memory_new();
int object_memory_delete(object_memory om);
int object_memory_used(object_memory om);
int object_memory_collect(object_memory om, GPtrArray *roots);
void object_memory_check_memory(object_memory om);
OBJECT object_memory_new_object(object_memory om, OBJECT cls, int fields);
void object_memory_print_stats(object_memory om);
OBJECT object_memory_new_opaque();

#endif

