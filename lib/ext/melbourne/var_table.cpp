#include <vector>
#include <stdlib.h>
#include <assert.h>

#include "ruby.h"
#include "var_table.hpp"

namespace melbourne {

  struct var_table_t {
    struct var_table_t *next;
    std::vector<quark> *quarks;
  };

  var_table var_table_create() {
    var_table vt = ALLOC(struct var_table_t);
    vt->quarks = new std::vector<quark>();
    vt->next = NULL;
    return vt;
  }

  void var_table_destroy(var_table vt) {
    while (vt) {
      var_table cur = vt;
      delete cur->quarks;
      vt = vt->next;
      free(cur);
    }
  }

  var_table var_table_push(var_table cur) {
    var_table vt = var_table_create();
    vt->next = cur;
    return vt;
  }

  var_table var_table_pop(var_table cur) {
    var_table nw;

    delete cur->quarks;
    nw = cur->next;
    free(cur);
    return nw;
  }

  int var_table_find(const var_table tbl, const quark needle) {
    for(size_t i = 0; i < tbl->quarks->size(); i++) {
      if(tbl->quarks->at(i) == needle) return i;
    }
    return -1;
  }

  int var_table_find_chained(const var_table tbl, const quark needle) {
    for(size_t i = 0; i < tbl->quarks->size(); i++) {
      if(tbl->quarks->at(i) == needle) return i;
    }

    if(tbl->next) {
      return var_table_find_chained(tbl->next, needle);
    }
    return -1;
  }

  int var_table_add(var_table tbl, const quark item) {
    tbl->quarks->push_back(item);
    return tbl->quarks->size();
  }

  int var_table_size(const var_table tbl)
  {
    return tbl->quarks->size();
  }

  quark var_table_get(const var_table tbl, const int index)
  {
    return tbl->quarks->at(index);
  }
};
