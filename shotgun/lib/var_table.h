#include <glib.h>

struct _var_table {
  GQuark data[128]; /* max number of vars, made up. */
  int size;
  struct _var_table *next;
};

typedef struct _var_table var_table;

var_table *var_table_create();
var_table *var_table_push(var_table *cur);
var_table *var_table_pop(var_table *cur);
int var_table_find(var_table *tbl, GQuark needle);
int var_table_add(var_table *tbl, GQuark item);