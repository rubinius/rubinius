#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/cpu.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/module.h"
#include "shotgun/lib/class.h"
#include "shotgun/lib/hash.h"
#include "shotgun/lib/lookuptable.h"
#include "shotgun/lib/methctx.h"
#include "shotgun/lib/array.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/symbol.h"
#include "shotgun/lib/machine.h"
#include "shotgun/lib/bytearray.h"
#include "shotgun/lib/fixnum.h"
#include "shotgun/lib/sendsite.h"
#include "shotgun/lib/subtend/ffi.h"
#include "shotgun/lib/subtend/nmc.h"

#if CONFIG_ENABLE_DTRACE
#include "shotgun/lib/dtrace_probes.h"

/*
 * Helper prototype
 */

void dtrace_source_context(STATE, cpu c, int*, const char**);


/*
 * Function entry/exit probe implementations
 */

void dtrace_function_primitive_entry(STATE, cpu c, const struct message *msg) {
  const char *module_name = msg->module == Qnil ? "<unknown>" : rbs_symbol_to_cstring(state, module_get_name(msg->module));
  const char *method_name = rbs_symbol_to_cstring(state, msg->name);

  cpu_flush_ip(c);

  int line_number; const char *filename;
  dtrace_source_context(state, c, &line_number, &filename);

  RUBINIUS_FUNCTION_PRIMITIVE_ENTRY((char*)module_name, (char*)method_name, (char*)filename, line_number);
}

void dtrace_function_primitive_return(STATE, cpu c, const struct message *msg) {
  const char *module_name = msg->module == Qnil ? "<unknown>" : rbs_symbol_to_cstring(state, module_get_name(msg->module));
  const char *method_name = rbs_symbol_to_cstring(state, msg->name);

  cpu_flush_ip(c);

  int line_number; const char *filename;
  dtrace_source_context(state, c, &line_number, &filename);

  RUBINIUS_FUNCTION_PRIMITIVE_RETURN((char*)module_name, (char*)method_name, (char*)filename, line_number);  
}

void dtrace_function_entry(STATE, cpu c, const struct message *msg) {
  const char *module_name = msg->module == Qnil ? "<unknown>" : rbs_symbol_to_cstring(state, module_get_name(msg->module));
  const char *method_name = rbs_symbol_to_cstring(state, msg->name);

  cpu_flush_ip(c);

  if (!NIL_P(c->active_context)) {
    int line_number; const char *filename;
    dtrace_source_context(state, c, &line_number, &filename);

    RUBINIUS_FUNCTION_ENTRY((char*)module_name, (char*)method_name, (char*)filename, line_number);
  } else {
    RUBINIUS_FUNCTION_ENTRY((char*)module_name, (char*)method_name, (char*)"-", 0); // in cases where there's no active context, ie. bootup
  }
}

void dtrace_function_return(STATE, cpu c) {
  OBJECT module = cpu_current_module(state, c);

  const char *module_name = (module == Qnil) ? "<unknown>" : rbs_symbol_to_cstring(state, module_get_name(module));
  const char *method_name = rbs_symbol_to_cstring(state, cmethod_get_name(cpu_current_method(state, c)));

  cpu_flush_ip(c);

  int line_number; const char *filename;
  dtrace_source_context(state, c, &line_number, &filename);

  RUBINIUS_FUNCTION_RETURN((char*)module_name, (char*)method_name, (char*)filename, line_number);
}


/*
 * Memory/GC probe implementations
 */

void dtrace_gc_begin(STATE) {  
  // Young generation stats
  int young_allocated = baker_gc_memory_allocated(state->om->gc);
  int young_in_use    = baker_gc_memory_in_use(state->om->gc);
  int young_obj_count = baker_gc_used(state->om->gc);
  
  // Mature generations stats
  
  RUBINIUS_GC_BEGIN(young_allocated, young_in_use, young_obj_count);
  
  baker_gc_reset_used(state->om->gc);
}

void dtrace_gc_end(STATE) {
  // Young generation stats
  int young_allocated = baker_gc_memory_allocated(state->om->gc);
  int young_in_use    = baker_gc_memory_in_use(state->om->gc);
  int young_obj_count = baker_gc_used(state->om->gc);
  
  // Mature generations stats
  
  RUBINIUS_GC_END(young_allocated, young_in_use, young_obj_count);
}

void object_create_start(OBJECT cls) {
  RUBINIUS_OBJECT_CREATE_START((char*)_inspect(cls), (char*)"unknown.rb", 1);
}

void object_create_done(OBJECT cls) {
  RUBINIUS_OBJECT_CREATE_DONE((char*)_inspect(cls), (char*)"unknown.rb", 1);
}


/*
 * Helpers
 */

void dtrace_source_context(STATE, cpu c, int *line_number, const char **filename) {
  struct fast_context *fc = FASTCTX(c->active_context);
  *line_number = cpu_ip2line(state, fc->method, fc->ip);
  *filename = rbs_symbol_to_cstring(state, cmethod_get_file(fc->method));  
}

#endif