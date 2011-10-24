#ifndef MEL_VAR_TABLE18_HPP
#define MEL_VAR_TABLE18_HPP

#include "quark.hpp"

#ifdef __cplusplus
extern "C" {
#endif


namespace melbourne {
  namespace grammar18 {
    struct var_table_t;
    typedef struct var_table_t *var_table;

    var_table var_table_create();
    void var_table_destroy(var_table vt);
    var_table var_table_push(var_table cur);
    var_table var_table_pop(var_table cur);
    int var_table_find(const var_table tbl, const quark needle);
    int var_table_find_chained(const var_table tbl, const quark needle);

    int var_table_add(var_table tbl, const quark item);
    int var_table_size(const var_table tbl);
    quark var_table_get(const var_table tbl, const int index);
  };
};

#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif
