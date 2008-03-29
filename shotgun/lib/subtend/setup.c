#include "ltdl.h"

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/subtend/nmc.h"
#include "shotgun/lib/subtend/ffi.h"

void subtend_setup_global();

rni_handle_table *handle_table_new();

/* The default library for dynamic loading--this process itself.
 * It is stored here hidden away to avoid having to reattach to
 * it every time.
 *
 * The internal variable is abstracted through a const pointer
 * to const for that extra bit of protection and inlinability.
 *
 * This could certainly have a place in STATE, but it should do
 * fine out here for now. I did not feel like fighting to get a
 * function pointing to a static inlined so variables it is.
 */
static ffi_dlhandle_t __ffi_handle_to_self;
const ffi_dlhandle_t* const ffi_this_process = &__ffi_handle_to_self;


void subtend_setup(STATE) {
  /* SystemData is class that just means that the body is opaque
     and not directly usable. It's used to store rni_nmc's
     native_method's */

  BASIC_CLASS(data) = rbs_class_new(state, "SystemData", 0, BASIC_CLASS(object));
  BASIC_CLASS(nmc) =  rbs_class_new(state, "NativeMethodContext", 0, BASIC_CLASS(fastctx));
  BASIC_CLASS(nmethod) = rbs_class_new(state, "NativeMethod", 0, BASIC_CLASS(cmethod));

  class_set_object_type(BASIC_CLASS(nmethod), I2N(NMethodType));

  ffi_dlinit();
  __ffi_handle_to_self = ffi_dlopen(NULL);  /* Obtain a handle to *this process* for default */

  subtend_setup_global();

  state->handle_tbl = handle_table_new();
}
