#include "shotgun.h"
#include "var_table.h"
#include <stdlib.h>
#include <assert.h>

/* max number of vars, made up. */
#define DATA_MAX 128

struct var_table_t {
  struct var_table_t *next;
  int size;
  quark data[DATA_MAX];
};

var_table var_table_create() {
  var_table vt = ALLOC(struct var_table_t);
  vt->size = 0;
  vt->next = NULL;
  return vt;
}

void var_table_destroy(var_table vt) {
  while (vt) {
    var_table cur = vt;
    vt = vt->next;

    XFREE(cur);
  }
}

var_table var_table_push(var_table cur) {
  var_table vt = var_table_create();
  vt->next = cur;
  return vt;
}

var_table var_table_pop(var_table cur) {
  var_table nw;
  
  nw = cur->next;
  XFREE(cur);
  return nw; 
}

int var_table_find(const var_table tbl, const quark needle) {
  int i;
  for(i = 0; i < tbl->size; i++) {
    if(tbl->data[i] == needle) return i;
  }
  return -1;
}

int var_table_add(var_table tbl, const quark item) {
  int idx;
  idx = tbl->size;
  assert(idx < DATA_MAX);
  tbl->data[idx] = item;
  tbl->size++;
  return idx;
}

int var_table_size(const var_table tbl)
{
  return tbl->size;
}

quark var_table_get(const var_table tbl, const int index)
{
  assert(index < tbl->size);
  return tbl->data[index];
}
