#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <assert.h>

#include "ruby.h"
#include "var_table.hpp"

namespace melbourne {

/* max number of vars */
/* value is slightly less made up these days, turns out that */
/* the old value of 128 was not enough to compile soap4r */
#define DATA_MAX 1024

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

      xfree(cur);
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
    xfree(cur);
    return nw;
  }

  int var_table_find(const var_table tbl, const quark needle) {
    int i;
    for(i = 0; i < tbl->size; i++) {
      if(tbl->data[i] == needle) return i;
    }
    return -1;
  }

  int var_table_find_chained(const var_table tbl, const quark needle) {
    int i;
    for(i = 0; i < tbl->size; i++) {
      if(tbl->data[i] == needle) return i;
    }

    if(tbl->next) {
      return var_table_find_chained(tbl->next, needle);
    }
    return -1;
  }

  int var_table_remove(var_table tbl, const quark needle) {
    int i;
    for(i = 0; i < tbl->size; i++) {
      if(tbl->data[i] == needle) {
        tbl->data[i] = -1;
        return i;
      }
    }
    return -1;
  }

  int var_table_add(var_table tbl, const quark item) {
    int idx;
    idx = tbl->size;
    if(idx >= DATA_MAX) {
      var_table_destroy(tbl);
      rb_raise(rb_eLoadError, "more than %i variables used", DATA_MAX);
      return -1;
    }
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

  void var_table_subtract(var_table tbl, var_table sub) {
    int sz = var_table_size(sub);
    for(int i = 0; i < sz; i++) {
      var_table_remove(tbl, var_table_get(sub, i));
    }
  }
};

#ifdef __cplusplus
}  /* extern "C" { */
#endif
