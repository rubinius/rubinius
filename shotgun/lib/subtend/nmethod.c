#include "shotgun.h"
#include "nmethod.h"
#include "symbol.h"
#include "object.h"
#include "ffi.h"

#include "primitive_indexes.h"

OBJECT nmethod_new(STATE, OBJECT mod, const char *file, const char *name, void *func, int args) {
  /* 40 argumennts is a lot, and the max (1.8 has a max of 13) */
  char params[40];
  OBJECT sys, nm;
  native_method *sys_nm;
  
  /* TODO: raise an exception. */
  if(args > 40) {
    return Qnil;
  }
    
  sys = object_memory_new_object_mature(state->om, BASIC_CLASS(data),
        BYTES2FIELDS(sizeof(native_method)));
  object_make_byte_storage(state, sys);
  
  sys_nm = (native_method*)BYTES_OF(sys);

  sys_nm->entry = func;
  sys_nm->args = args;
    
  sys_nm->stub = ffi_generate_c_stub(state, args, func);
  
  nm = nmethod_allocate(state);
  cmethod_set_primitive(nm, I2N(CPU_PRIMITIVE_NMETHOD_CALL));
  cmethod_set_required(nm, I2N(args));
  cmethod_set_serial(nm, I2N(0));
  cmethod_set_name(nm, symtbl_lookup_cstr(state, state->global->symbols, name));
  cmethod_set_file(nm, symtbl_lookup_cstr(state, state->global->symbols, file));
  nmethod_set_data(nm, sys);
  
  return nm;
}

