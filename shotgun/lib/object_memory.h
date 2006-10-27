#include "rubinius.h"
#include "baker.h"
#include <glib.h>

#ifndef __RUBINIUS_OM__ 1
#define __RUBINIUS_OM__ 1

// #define OMDefaultSize 384000
#define OMDefaultSize 6000000

struct object_memory_struct {
  baker_gc gc;  
};

typedef struct object_memory_struct *object_memory;

object_memory object_memory_new();
int object_memory_delete(object_memory om);
int object_memory_used(object_memory om);
int object_memory_collect(object_memory om, GPtrArray *roots);
OBJECT object_memory_new_object(object_memory om, OBJECT cls, int fields);
void object_memory_print_stats(object_memory om);

#endif