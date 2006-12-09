#include "var_table.h"
#include <stdlib.h>
#include <assert.h>

var_table *var_table_create() {
  var_table *vt = malloc(sizeof(var_table));
  vt->size = 0;
  vt->next = NULL;
  return vt;
}

var_table *var_table_push(var_table *cur) {
  var_table *vt = var_table_create();
  vt->next = cur;
  return vt;
}

var_table *var_table_pop(var_table *cur) {
  var_table *nw;
  
  nw = cur->next;
  free(cur);
  return nw; 
}

int var_table_find(var_table *tbl, GQuark needle) {
  int i;
  for(i = 0; i < tbl->size; i++) {
    if(tbl->data[i] == needle) return i;
  }
  return -1;
}

int var_table_add(var_table *tbl, GQuark item) {
  int idx;
  idx = tbl->size;
  assert(idx < 128);
  tbl->data[idx] = item;
  tbl->size++;
  return idx;
}
