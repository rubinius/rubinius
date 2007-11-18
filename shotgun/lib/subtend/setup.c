#include "shotgun.h"
#include "nmc.h"

#include "ltdl.h"

void subtend_setup_global();

rni_handle_table *handle_table_new();

void subtend_setup(STATE) {
  /* SystemData is class that just means that the body is opaque
     and not directly usable. It's used to store rni_nmc's
     native_method's */
  
  BASIC_CLASS(data) = rbs_class_new(state, "SystemData", 0, BASIC_CLASS(object));
  BASIC_CLASS(nmc) =  rbs_class_new(state, "NativeMethodContext", 0, BASIC_CLASS(fastctx));
  BASIC_CLASS(nmethod) = rbs_class_new(state, "NativeMethod", 0, BASIC_CLASS(cmethod));
  
  lt_dlinit();
  subtend_setup_global();
  
  state->handle_tbl = handle_table_new();
}
