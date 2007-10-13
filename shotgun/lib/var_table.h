#include <quark.h>

struct _var_table {
  quark data[128]; /* max number of vars, made up. */
  int size;
  struct _var_table *next;
};

typedef struct _var_table var_table;

var_table *var_table_create();
void var_table_destroy(var_table *vt);
var_table *var_table_push(var_table *cur);
var_table *var_table_pop(var_table *cur);
int var_table_find(var_table *tbl, quark needle);
int var_table_add(var_table *tbl, quark item);
