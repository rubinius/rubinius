#ifndef MEL_LOCALSTATE_HPP
#define MEL_LOCALSTATE_HPP

#include "var_table18.hpp"

namespace melbourne {
  namespace grammar18 {
    struct LocalState {
      LocalState* prev;
      var_table local_vars;
      var_table block_vars;

      LocalState(LocalState* prev)
        : prev(prev)
        , local_vars(var_table_create())
        , block_vars(0)
      {}

      ~LocalState() {
        var_table_destroy(local_vars);
        if(block_vars) {
          var_table_destroy(block_vars);
        }
      }

      bool blocks_p() {
        return block_vars != NULL;
      }

      static LocalState* push(LocalState* cur) {
        return new LocalState(cur);
      }

      static LocalState* pop(LocalState* cur) {
        LocalState* tmp = cur->prev;
        delete cur;
        return tmp;
      }
    };
  };
};

#endif
