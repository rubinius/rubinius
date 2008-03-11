#include "shotgun/dtrace.h"

#define ENABLE_DTRACE 1

/* function entry/exit probes */

void dtrace_function_primitive_entry(STATE, cpu c, const struct message *msg);
void dtrace_function_primitive_return(STATE, cpu c, const struct message *msg);
void dtrace_function_entry(STATE, cpu c, const struct message *msg);
void dtrace_function_return(STATE, cpu c);

/* memory management probes */
void dtrace_gc_begin();
void dtrace_gc_end();
void object_create_start(OBJECT cls);
void object_create_done(OBJECT cls);
