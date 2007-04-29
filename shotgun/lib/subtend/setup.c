#include "shotgun.h"
#include "nmc.h"

#ifdef USE_CINVOKE
#include <cinvoke.h>
#endif

#include "ltdl.h"

void subtend_setup_global();

void subtend_setup(STATE) {
  /* SystemData is class that just means that the body is opaque
     and not directly usable. It's used to store rni_nmc's
     native_method's */
  
  BASIC_CLASS(data) = rbs_class_new(state, "SystemData", 0, BASIC_CLASS(object));
  BASIC_CLASS(nmc) =  rbs_class_new(state, "NativeMethodContext", 0, BASIC_CLASS(fastctx));
  BASIC_CLASS(nmethod) = rbs_class_new(state, "NativeMethod", 0, BASIC_CLASS(cmethod));
  
  lt_dlinit();
#ifdef USE_CINVOKE
  state->c_context = cinv_context_create();
#endif
  subtend_setup_global();
  
  state->handle_tbl = handle_table_new();
}
