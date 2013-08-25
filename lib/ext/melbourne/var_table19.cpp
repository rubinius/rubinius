#include "melbourne.hpp"
#include "var_table19.hpp"

namespace melbourne {
  namespace grammar19 {
    int vtable_size(const struct vtable* tbl) {
      if(tbl) {
        return tbl->pos;
      } else {
        return 0;
      }
    }

    struct vtable* vtable_alloc(struct vtable *prev) {
      struct vtable *tbl = ALLOC(struct vtable);
      tbl->pos = 0;
      tbl->capa = 8;
      tbl->tbl = ALLOC_N(ID, tbl->capa);
      tbl->prev = prev;
      return tbl;
    }

    void vtable_free(struct vtable* tbl) {
      if(tbl) {
        if(tbl->tbl) {
          xfree(tbl->tbl);
        }
        xfree(tbl);
      }
    }

    void vtable_free_all(struct vtable* tbl) {
      if(tbl) {
        if(tbl->tbl) {
          xfree(tbl->tbl);
        }
        if(tbl->prev) {
          vtable_free_all(tbl->prev);
        }
        xfree(tbl);
      }
    }

    void vtable_add(struct vtable* tbl, ID id) {
      if(!tbl) {
        rb_bug("vtable_add: vtable is not allocated (%p)", (void *)tbl);
      }

      if(tbl->pos == tbl->capa) {
        tbl->capa = tbl->capa * 2;
        REALLOC_N(tbl->tbl, ID, tbl->capa);
      }
      tbl->tbl[tbl->pos++] = id;
    }

    bool vtable_included(const struct vtable* tbl, ID id) {
      if(tbl) {
        for(int i = 0; i < tbl->pos; i++) {
          if(tbl->tbl[i] == id) {
            return true;
          }
        }
      }
      return false;
    }

    void local_vars_free(struct local_vars* vars) {
      struct local_vars* prev;
      for(struct local_vars* local = vars; local; local = prev) {
        if(local->args) vtable_free_all(local->args);
        if(local->vars) vtable_free_all(local->vars);
        prev = local->prev;
        xfree(local);
      }
    }
  };
};
